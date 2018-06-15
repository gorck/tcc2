/*******************************************************************

    Classe principal contendo os imports e logica combinacional

    @autor Jose Augusto Gorck
    @since 23/03/18

*/

/***************** LIST OF PINS BEING USED *******************
   A0  -
   A1  -
   D0  - Bluetooth/RX
   D1  - Bluetooth/TX
   D4  -
*/

/********************** LIBS *********************/
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


/********************** GLOBAL VARIABLES *********************/
#define SERIAL_BAUD_RATE 9600       //Serial baud rate 

#define SOH 0x01 //start of transmission
#define STX 0x02 //start of text
#define ETX 0x03 //end of text
#define EOT 0x04 //end of transmission
#define ETB 0x17 //end od trans block
#define FS 0x1C //file separator

/*
   Discovery Comands Type
*/
#define CMD_DISCOVERY 0x01 // gateway discovery
#define CMD_DISCOVERY_ACK 0x02 // gateway response of discovery

/*
   Request and Response Comands Type
*/
#define CMD_REQUEST_NODE_STATUS 0x03 // Requisição status do nodo
#define CMD_RESPONSE_NODE_STATUS 0x04 // Requisição de dados do sensor
#define CMD_REQUEST_NODE_DATA 0x05 // Requisição status do nodo
#define CMD_RESPONSE_NODE_DATA 0x06 // Requisição de dados do sensor


#define CMD_DATA 0x0A // novo dado

#define DISCOVERY_PACKET_SIZE 10 // busca novo IP


#define SENDOR_TYPE 0x0A;

unsigned long last_update = 0;

boolean gateway_discovered = false;

byte mac[] = {0x1B, 0xAB, 0xDC, 0x1A, 0x11, 0x1A};
byte macServer[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

IPAddress broadcast_ip(255, 255, 255, 255);
IPAddress server_ip;

int discovery_response_port = 6666;
int discovery_port = 6667;

EthernetUDP udp;

const int BUFFER_PACKET_SIZE = 48;
byte packetBuffer[BUFFER_PACKET_SIZE];
byte replayBuffer[BUFFER_PACKET_SIZE];

int states_ip_discovery = 0;

struct state {
  unsigned long last_data_sent;
  int status_discovery;
  int action;
  union {
    float data;
    unsigned char bytes_data[4];
  } last_data_sensor;
};
state KDState;

void setup() {

  Ethernet.begin(mac, IPAddress(0, 0, 0, 0));
  Serial.begin(9600);
  Serial.println("START UP Sensoriamento");
  Serial.print("Iniciando UDP na porta:");
  Serial.println(discovery_response_port);
  KDState.status_discovery = 0;
  KDState.action = 1;
  udp.begin(discovery_response_port);
  Serial.println("START UP");

}

void loop() {

  int packetSize = udp.parsePacket();
  if (packetSize) {

    Serial.print("Resposta do servidor: ");
    Serial.println(udp.remoteIP());
    udp.read(packetBuffer, BUFFER_PACKET_SIZE);
    Serial.println("Pacote recebido: ");
    for (int i = 0; i < BUFFER_PACKET_SIZE; i++) {
      Serial.print(packetBuffer[i], HEX);
    }
    Serial.println(" ");
    wrapperAnswer();
  }
  stateMachine();
}


void stateMachine() {
  switch (KDState.action) {
    case 1: //discoveryng IP
    case 2: //discoveryng IP
      Serial.print("KDState.action =");
      Serial.println(KDState.action);
      sMDiscoveryIP();
      break;
    case CMD_REQUEST_NODE_DATA: //get data sensor
      Serial.print("KDState.action: ");
      Serial.println(CMD_REQUEST_NODE_DATA);
      KDState.last_data_sensor.data = getSensorData();
      sendDataSensor();
      delay(200);
      sendNodeStatus(0);
      KDState.action = 8;
      break;
    case CMD_RESPONSE_NODE_STATUS: //message that informs the sensor node is awake or alive
      Serial.print("KDState.action: ");
      Serial.println(CMD_RESPONSE_NODE_STATUS);
      sendNodeStatus(1);
      KDState.action = 0;
      break;
    case 8: //message that informs the sensor node is going to sleep
      Serial.print("KDState.action: ");
      Serial.println(8);
      sendNodeStatus(0);
      KDState.action = 0;
      break;
    default:
      break;
  }
}


void sMDiscoveryIP() {
  Serial.println("sMDiscoveryIP()");
  Serial.print("KDState.status_discovery =");
  Serial.println(KDState.status_discovery);
  switch (KDState.status_discovery) {
    case 0: // busca IP
      discoveryGateway();
      delay(200);
      break;
    case 1:// Autenticação recebida
      KDState.status_discovery = 4; //concluido
      KDState.action = CMD_RESPONSE_NODE_STATUS;
      break;
  }
}







