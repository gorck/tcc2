/*******************************************************************

    Classe principal contendo os imports e logica combinacional

    @autor Jose Augusto Gorck
    @since 23/03/18

*/

/********************** LIBS *********************/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <stdio.h>



/********************** DEFINE VARIABLES *********************/

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


#define CMD_LIST_SENSOR 0x0B // Lista de sensores recebida pelo gateway
#define CMD_DATA 0x0A // novo dado

#define SENDOR_TYPE  99

#define MAX_DATA_SENSOR 5
#define MAX_SENSOR 5


/********************** GLOBAL VARIABLES *********************/
struct DataInfo {
  unsigned long timeAquisition = 0;
  int info = 0;
};


struct Sensor {
  unsigned long timeToGetData = 0;
  unsigned long lastTimeData = 0;
  int typeData = 0;
  byte mac[6];
  bool isAwake = false;
  DataInfo dataInfo [MAX_DATA_SENSOR];
};



byte server[] = { 192, 168, 0, 111 };
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA};

byte ipOffer[] = { 192, 168, 0, 11};
byte ipOffer12[] = { 192, 168, 0, 12};
byte ipOffer13[] = { 192, 168, 0, 13};


const int BUFFER_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[BUFFER_PACKET_SIZE];
byte replayBuffer[BUFFER_PACKET_SIZE];

Sensor sensorList[MAX_SENSOR];


//unsigned int localPort = 1903;
unsigned int remPort = 5544;

unsigned int discoveryPort = 6667;

unsigned int dataPort = 6666;

EthernetUDP udp; // discoveryServer

IPAddress broadcastIP(255, 255, 255, 255);
IPAddress newSensorIP;

void setup() {
  Serial.begin(9600);
  // start the Ethernet and UDP:
  Serial.println("START UP Centralizador");
  Ethernet.begin(mac, IPAddress(192, 168, 0, 222));
  Serial.print("Iniciando UDP na porta:");
  Serial.println(discoveryPort);
  udp.begin(discoveryPort);

  memset(sensorList, 0, sizeof(sensorList));

  Serial.println("START UP Finalizado");
}

void loop() {
  udp.begin(discoveryPort);
  if (udp.parsePacket()) {
    Serial.println("Recebido novo pacote ");
    newPacketRecived();
  }


    requestData();
}

void newPacketRecived() {
  Serial.println("Novo Pacote........");
  udp.read(packetBuffer, BUFFER_PACKET_SIZE);

  int action = getCmdMessage();
  Serial.print("Action:");
  Serial.println(action);

  switch (action) {            //identify and apply the command
    case 0: // ipDiscovery
      //do nothing
      break;
    case CMD_DISCOVERY: // ipDiscoveryf
      sendAutenticationAck();
      break;
    case CMD_RESPONSE_NODE_STATUS: // CMD_REQUEST_DATA
      processNodeStatus();
      break;
    case 10:// new data sensro
      break;
    default:
      break;
  }
}




void requestData() {
  //  Serial.print("###TAMANHO DA LISTA" );
  //  Serial.println(sizeof(sensorList));
  for (int i = 0; i < MAX_SENSOR; i++) {
    if (sensorList[i].isAwake) {
      Serial.print("Requisitando dados ao sensor: ");
      Serial.print(i);
      Serial.print("isAwake: ");
      Serial.println( sensorList[i].isAwake);
      makePacketToGetData(i);

    }
  }
  delay(500);
}






