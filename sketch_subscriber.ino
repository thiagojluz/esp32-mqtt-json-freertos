/* * Universidade Federal do Paraná (UFPR) - DELT
 * Disciplina: Sistemas Operacionais Embarcados
 * Prática 5: Subscriber MQTT com Parsing de JSON
 * Professor: Thiago José da Luz, PhD 
 * Data: 28/03/2026
 * * OBJETIVO DIDÁTICO: 
 * Implementar um nó receptor (Subscriber) que utiliza o protocolo MQTT para 
 * consumir dados serializados em JSON. O código demonstra o uso de funções 
 * de callback para processamento assíncrono e a integração de bibliotecas 
 * de desserialização para extração de dados estruturados.
 */
// Subscriber

#include <WiFi.h> //para usar o Wifi
#include <PubSubClient.h>
#include <ArduinoJson.h> // Biblioteca essencial para trabalhar com JSON
#include <LiquidCrystal.h>

// Parâmetros de Conectividade
const char* SSID =  "Wokwi-GUEST"; // nome da rede wifi virtual do WokWi
const char* PASSWORD = ""; // senha
WiFiClient wifiClient;   // cria o objeto wifiClient

//MQTT Server
const char* BROKER_MQTT = "broker.hivemq.com"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883;                      // Porta do Broker MQTT

// Identificadores Únicos (Importante para evitar conflitos no Broker)
#define ID_MQTT  "TL02"            //Informe um ID unico e seu. Caso sejam usados IDs repetidos a ultima conexão irá sobrepor a anterior. 
#define TOPIC_SUBSCRIBE "TLTemp"    //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.
PubSubClient MQTT(wifiClient);        // Instancia o Cliente MQTT passando o objeto espClient

// Barramento Paralelo do LCD (Modo 4 bits)
#define RS 19
#define E 18
#define D4 5
#define D5 17
#define D6 16
#define D7 4
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);


// Protótipos de Gestão de Conexão
void mantemConexoes();  //Garante que as conexoes com WiFi e MQTT Broker se mantenham ativas
void conectaWiFi();     //Faz conexão com WiFi
void conectaMQTT();     //Faz conexão com Broker MQTT
void recebePacote(char* topic, byte* payload, unsigned int length);

void setup() {

  Serial.begin(115200);

  conectaWiFi(); // conectar com WIfi
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   // Conectar com o Broker
  MQTT.setCallback(recebePacote); 

  // Inicialização de Periféricos
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("Iniciando...");
}

void loop() {
  // Mantém a resiliência das conexões em tempo de execução
  mantemConexoes();
  /* MQTT Loop: Processa mensagens de entrada e mantém o 'keep-alive' 
     com o Broker. Fundamental para o funcionamento do Callback. */
  MQTT.loop();
}

/* --- FUNÇÃO DE CALLBACK (PROCESSAMENTO DE DADOS) --- */
void recebePacote(char* topic, byte* payload, unsigned int length) 
{
    /* Alocação de buffer para o Documento JSON. 
     StaticJsonDocument é mais rápido e adequado para mensagens pequenas. */
    StaticJsonDocument<200> doc; 

    // Desserialização: Transforma o payload (bytes) em um objeto JSON acessível
    DeserializationError error = deserializeJson(doc, payload, length);

    if (error) {
      Serial.print("Falha na desserialização: ");
      Serial.println(error.c_str());
      return;
    }

    // Extração Tipada: Acesso direto às chaves do JSON
    float temperatura = doc["temperatura"]; // Pega o valor associado à chave "temperatura"
    const char* unidade = doc["unidade"]; // Para strings, use const char*

    // Atualização da Interface Local
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp. Recebida:");
    
    lcd.setCursor(0, 1);
    lcd.print(temperatura);
    lcd.print(" ");
    lcd.print(unidade); 
    
    // Log Serial para Debug
    Serial.printf("Pacote recebido via MQTT [%s]: %.2f %s\n", topic, temperatura, unidade);
}

/* --- GESTÃO DE REDE E CONEXÕES --- */
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
            MQTT.subscribe(TOPIC_SUBSCRIBE);
        } 
        else {
            Serial.println("Noo foi possivel se conectar ao broker.");
            Serial.println("Nova tentatica de conexao em 10s");
            delay(10000);
        }
    }
}