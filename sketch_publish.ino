/* Universidade Federal do Paraná (UFPR) - DELT
  * Disciplina: Sistemas Operacionais Embarcados
  * Prática 5: Comunicação MQTT com Serialização JSON e Queues
  * Professor: Thiago José da Luz, PhD 
  * Data: 28/03/2026
  * OBJETIVO DIDÁTICO: 
  * Demonstrar o padrão Produtor-Consumidor usando FreeRTOS Queues. 
  * Uma tarefa (Produtor) realiza a leitura determinística do sensor, enquanto 
  * outra tarefa (Consumidor) gerencia a pilha de protocolos de rede e 
  * a persistência da conexão MQTT, garantindo que falhas de rede não 
  * bloqueiem a aquisição de dados.
*/
// Publish

#include <WiFi.h> //para usar o Wifi
#include <PubSubClient.h>

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/queue.h>

#include <ArduinoJson.h> // Biblioteca essencial para trabalhar com JSON
#include <DHTesp.h>      // Biblioteca do sensor DHT22

// Parâmetros da Rede e Broker
const char* SSID =  "Wokwi-GUEST"; // nome da rede wifi virtual do WokWi
const char* PASSWORD = ""; // senha
WiFiClient wifiClient;   // cria o objeto wifiClient

//MQTT Server
const char* BROKER_MQTT = "broker.hivemq.com"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT
//outra opção test.mosquitto.org

#define ID_MQTT  "TL01"        //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_PUBLISH "TLTemp" //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

#define temp 15
#define DHT_PIN temp
DHTesp dhtSensor;

// // Handlers do FreeRTOS
QueueHandle_t xFila;
void TaskTemp(void *pvParameters);
void TaskSend(void *pvParameters);

// Protótipos de Gestão de Conexão
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT

void setup() {    

  Serial.begin(115200);

  conectaWiFi(); // conectar com WIfi
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   // Conectar com o Broker

  // Task 1: Pinned ao Core 0 - Enviar mensagem
  xTaskCreatePinnedToCore(TaskSend, "Enviar", 10000, NULL, 1, NULL,0);
  // Task 2: Pinned ao Core 1 - Foco em leitura de sensor digital (DHT22)
  xTaskCreatePinnedToCore(TaskTemp, "Temperatura", 10000, NULL, 1, NULL,1);
  
  // Inicialização de Periféricos
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);

  /* CRIAÇÃO DA FILA (QUEUE):
   * Atua como um buffer entre o núcleo de aquisição e o núcleo de transmissão.
   * Evita perda de dados durante processos de reconquista de conexão (reconnect).
   */
  xFila = xQueueCreate(
    10,   // Tamanho da fila
    2*sizeof(float)  // Tamanho do item (tipo float)
  );
  if(xFila) Serial.println("Fila criada com sucesso!");
}

void loop() {
  
}

/* --- TAREFA PRODUTORA (Core 1) --- */
void TaskTemp(void *pvParameters) {
  (void) pvParameters;
  float temperatura;

  for(;;){

    // lê o sensor
    TempAndHumidity  data = dhtSensor.getTempAndHumidity();
    temperatura = data.temperature;
    // Envia o dado para o fim da fila. Aguarda indefinidamente se a fila estiver cheia.
    xQueueSendToBack( xFila, &temperatura, portMAX_DELAY);
    Serial.println(temperatura);
    vTaskDelay(500/portTICK_PERIOD_MS); // Bloqueio da Task por 500ms
  }
}

/* --- TAREFA CONSUMIDORA (Core 0) --- */
void TaskSend(void *pvParameters) {
  (void) pvParameters;
  float temperatura;
  
  StaticJsonDocument<200> doc; // // Alocação de memória para serialização JSON
  
  for(;;){
    // Garante que o sistema esteja online antes de processar a fila
    mantemConexoes();
    // Bloqueia a task até que um dado esteja disponível na fila
    xQueueReceive(xFila, &temperatura, portMAX_DELAY);
    
    // Estruturação do Payload JSON para interoperabilidade industrial
    doc["temperatura"] = temperatura;
    doc["unidade"] = "C";

    char payload[200];
    serializeJson(doc, payload);
    
    // Publicação via MQTT
    MQTT.publish(TOPIC_PUBLISH, payload); // envia pacote
    Serial.println("MQTT Publish (Core 0): Sucesso");
    vTaskDelay(10/portTICK_PERIOD_MS); // Bloqueio da Task por 500ms
  }
}

/* --- GESTÃO DE CONECTIVIDADE --- */
void mantemConexoes() {
    if (!MQTT.connected()) {
       conectaMQTT(); 
    }
    
    conectaWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

void conectaWiFi() {

  if (WiFi.status() == WL_CONNECTED) {
     return;
  }
        
  Serial.print("Conectando-se na rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI  
  while (WiFi.status() != WL_CONNECTED) {
      delay(100);
      Serial.print(".");
  }
  
  Serial.println();
  Serial.print("Conectado com sucesso, na rede: ");
  Serial.print(SSID);  
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP()); 
}

void conectaMQTT() { 
    while (!MQTT.connected()) {
        Serial.print("Conectando ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) {
            Serial.println("Conectado ao Broker com sucesso!");
        } 
        else {
            Serial.println("Não foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}