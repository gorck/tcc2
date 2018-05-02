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


/********************** GLOBAL VARIABLES *********************/
#define SERIAL_BAUD_RATE 9600       //Serial baud rate 

#define SOH 0x01 //start of transmission
#define STX 0x02 //start of text
#define ETX 0x03 //end of text
#define EOT 0x04 //end of transmission
#define ETB 0x17 //end od trans block
#define FS 0x1C //file separator

#define CMD_DISCOVERY 0x00 // busca novo IP
#define CMD_DISCOVERY_OFFER 0x01 // Oferece um novo IP
#define CMD_DISCOVERY_REQUEST 0x02 // Requisita o IP
#define CMD_DISCOVERY_ACK 0x03 // Aceita o novo IP


#define CMD_DATA 0x0A // novo dado

#define DISCOVERY_PACKET_SIZE 10 // busca novo IP


#define SENDOR_TYPE 0x01;

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

long lastUpdate = 0;

boolean gatewayDiscovered = false;

byte ip[] = {192, 168, 0, 159};
byte mac[] = {0x1B, 0xAB, 0xDC, 0x1A, 0x11, 0x1A};

IPAddress broadcastIP(255, 255, 255, 255);



IPAddress serverIP;

unsigned int discoveryResponsePort = 6666;
unsigned int discoveryPort = 6667;

EthernetUDP udp;
const int BUFFER_PACKET_SIZE = 48;
byte packetBuffer[BUFFER_PACKET_SIZE];
byte replayBuffer[BUFFER_PACKET_SIZE];

int statesIpDiscovery = 0;




struct state {
  unsigned long lastDataSent;
  float lastDataSensor;
  int statusDiscovery;
  int action;
};
state KDState;

void setup() {
  startEth(mac, ip);
  Serial.begin(9600);
  Serial.print("Iniciando UDP na porta:");
  Serial.println(discoveryResponsePort);
  udp.begin(discoveryResponsePort);
  KDState.statusDiscovery = 0;
  KDState.action = 0;
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

  delay(200);
}


void stateMachine() {


  switch (KDState.action) {
    case 0: //discoveryng IP
      sMDiscoveryIP();
      break;
    case 1: //waiting schedule
      break;
    case 2: //get data sensor
      KDState.lastDataSensor = getSensorData();
      break;
    case 3: //send sensor data
      break;
    case 4: //send last data sensor
      break;
    case 5: //clear data sensor
      break;

  }

}


void sMDiscoveryIP() {
  switch (KDState.statusDiscovery) {
    case 0: // busca IP
      discoveryGateway();
      KDState.statusDiscovery = 1;
      break;
    case 1:// Oferta de IP recebida

      break;
    case 2: // Processo de obtenção de IP concluida
      sendCmdDiscoveryRequestPacket();
      KDState.statusDiscovery = 3;
      break;
    case 3: //ACK do servidor recebido
      KDState.statusDiscovery = 4;
      break;
  }
}







