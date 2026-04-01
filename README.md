
# Prática 5: Comunicação M2M com MQTT e JSON no FreeRTOS 📡🛰️

Este repositório apresenta o desenvolvimento da **Prática 5** da disciplina de  
**Sistemas Operacionais Embarcados**, ministrada no **Departamento de Engenharia Elétrica da Universidade Federal do Paraná (UFPR)**.

O projeto implementa uma **arquitetura de comunicação Machine-to-Machine (M2M)** entre **dois dispositivos ESP32**, utilizando o protocolo **MQTT** e serialização de dados em **JSON**, sobre uma base multitarefa com **FreeRTOS**.

Esta prática consolida os conceitos de **concorrência, desacoplamento, IoT e comunicação distribuída**, aproximando-se de aplicações reais de **telemetria industrial e sistemas ciber-físicos**.

---

## 🎯 Objetivos Didáticos

1. **Arquitetura Produtor–Consumidor:**  
   Utilização de **Filas (`xQueue`)** para desacoplar a tarefa de aquisição de dados do sensor da tarefa responsável pela comunicação em rede.

2. **Serialização de Dados:**  
   Uso da biblioteca **ArduinoJson** para estruturar as informações de forma padronizada, legível e extensível.

3. **Protocolo Publish/Subscribe:**  
   Implementação de comunicação via **MQTT**, utilizando um modelo de publicação e subscrição através de um **Broker (HiveMQ)**.

---

## 🧠 Competências Demonstradas

- Projeto de sistemas embarcados **distribuídos (M2M)**
- Comunicação baseada em **MQTT (Pub/Sub)**
- Serialização e desserialização de dados em **JSON**
- Implementação de arquitetura **Produtor–Consumidor** com FreeRTOS
- Uso de **Filas (Queues)** para desacoplamento funcional
- Integração entre sensores, processamento local e comunicação em rede
- Análise de robustez e determinismo em sistemas IoT

---

## 🛠️ Implementação Técnica

### Nó Publicador (ESP32 – Publisher)

- **TaskTemp:**  
  Responsável pela leitura periódica do sensor **DHT22**, enviando os dados para uma **Fila (`xQueue`)**.

- **Fila (Queue):**  
  Atua como buffer intermediário entre a aquisição de dados e a comunicação de rede, garantindo que a amostragem mantenha comportamento determinístico, mesmo durante atrasos ou reconexões WiFi/MQTT.

- **TaskSend:**  
  Consome os dados da fila, realiza a serialização em **JSON** e publica as mensagens no Broker MQTT. Os dados são enviados no formato `{"temperatura": 25.5, "unidade": "C"}`, facilitando a integração com sistemas de Big Data e Dashboards.

---

### Nó Assinante (ESP32 – Subscriber)

- Atua como **Subscriber MQTT**.
- Realiza o processamento (*parsing*) das mensagens JSON recebidas.
- Exibe os dados recebidos em tempo real em um **display LCD 16x2**.

---

## 🛠️ Hardware Utilizado (Simulado no Wokwi)

- **Microcontroladores:** ESP32 (Publisher e Subscriber)
- **Sensor:** DHT22 (Temperatura)
- **Display:** LCD 16x2
- **Comunicação:** MQTT sobre WiFi
- **Broker MQTT:** HiveMQ (público)

---

## 💡 Relevância Técnica

Arquiteturas **M2M baseadas em MQTT e JSON** são amplamente empregadas em aplicações reais como:

- telemetria industrial;
- sistemas de monitoramento distribuído;
- smart grids;
- IoT industrial (IIoT);
- sistemas SCADA modernos.

O uso de **Filas (Queues)** permite o desacoplamento entre aquisição de dados e comunicação em rede, aumentando a **robustez**, a **previsibilidade** e a **escalabilidade** do sistema diante de instabilidades temporárias de conexão.

---

## 🚀 Como Executar

1. O código principal está localizado no arquivo `sketch.ino`.
2. As conexões de hardware estão descritas no arquivo `diagram.json`.
3. As bibliotecas necessárias estão especificadas no arquivo `libraries.txt`.
4. **Simulação Online (Wokwi):**
    * [Publish](https://wokwi.com/projects/437209107393756161) 
    * [Subscriber](https://wokwi.com/projects/437211045922565121)

---

## 🧪 Validação e Testes

- Verificação do envio periódico de mensagens MQTT.
- Monitoramento da estabilidade da comunicação M2M.
- Testes de reconexão WiFi/MQTT sem perda de dados críticos.
- Validação do processamento correto das mensagens no nó assinante.
- Atualização contínua e estável das informações no display LCD.

---

## 📚 Evolução do Projeto

Esta prática conclui a série progressiva desenvolvida no portfólio:

- **Prática 1:** Sistemas reativos com interrupções (bare-metal)
- **Prática 2:** Concorrência e condição de corrida
- **Prática 3:** Exclusão mútua com Mutex
- **Prática 4:** Integração IoT com nuvem
- **Prática 5:** Comunicação M2M distribuída com MQTT e JSON

---

**Prof. Thiago José da Luz, PhD**  
Departamento de Engenharia Elétrica – UFPR
