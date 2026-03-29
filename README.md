# Prática 5: Comunicação M2M com MQTT e JSON no FreeRTOS 📡🛰️

### Este projeto implementa uma arquitetura de comunicação Machine-to-Machine (**M2M**) entre dois ESP32, utilizando o protocolo MQTT e serialização de dados em JSON.

## 🎯 Objetivos Didáticos
1. **Arquitetura Produtor-Consumidor:** Uso de `xQueue` para desacoplar a tarefa de amostragem do sensor da tarefa de comunicação de rede.

2. **Serialização de Dados:** Uso da biblioteca `ArduinoJson` para estruturar informações de forma legível e escalável.

3. **Protocolo Pub/Sub:** Implementação de publicação e subscrição via Broker MQTT (HiveMQ), simulando telemetria industrial.

# 🛠️ Implementação Técnica
* **Fila (Queue):** A `TaskTemp` lê o DHT22 e posta o valor na `xFila`. Isso permite que a amostragem mantenha o determinismo mesmo que a `TaskSend` esteja ocupada reestabelecendo a conexão WiFi/MQTT.

* **JSON Payload:** Os dados são enviados no formato `{"temperatura": 25.5, "unidade": "C"}`, facilitando a integração com sistemas de Big Data e Dashboards.

* **Subscriber:** O segundo ESP32 atua como receptor, realizando o *parsing* do JSON e exibindo os dados em tempo real em um LCD 16x2.

## 🚀 Como Executar
1. O código principal está localizado no arquivo sketch.ino.
2. As conexões de hardware estão descritas no arquivo diagram.json.
3. As bibliotecas estão especificadas no arquivo libraries.txt 
4. **Simulação Online:** Você pode testar o comportamento do sistema diretamente no navegador:
    * ▶️ Link para a Simulação no Wokwi:

    * [Publish](https://wokwi.com/projects/437209107393756161) 
    * [Subscriber](https://wokwi.com/projects/437211045922565121)


---
### Prof. Thiago José da Luz, PhD 