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


#define CMD_DISCOVERY 0x01 // busca novo IP
//#define CMD_DISCOVERY_OFFER 0x02 // Oferece um novo IP
//#define CMD_DISCOVERY_REQUEST 0x03 // Requisita o IP
#define CMD_DISCOVERY_ACK 0x04 // Aceita o novo IP

#define CMD_NODE_STATUS 0x05 // Requisição status do nodo
#define CMD_REQUEST_DATA 0x06 // Requisição de dados do sensor
#define CMD_RESPONSE_DATA 0x07 // Resposta contendo o dados do sensor

#define CMD_LIST_SENSOR 0x0B // Lista de sensores recebida pelo gateway

#define CMD_DATA 0x0A // novo dado

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
  bool alive = false;
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

unsigned int dataPort = 666;

EthernetUDP udp; // discoveryServer

IPAddress broadcastIP(255, 255, 255, 255);
IPAddress newSensorIP;

void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, IPAddress(192, 168, 0, 222));
  Serial.begin(9600);
  udp.begin(discoveryPort);

  memset(sensorList, 0, sizeof(sensorList));

}

void loop() {
  udp.begin(discoveryPort);
  if (udp.parsePacket()) {
    Serial.println("Recebido novo pacote ");
    newPacketRecived();
  }


//  requestData();
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
    case 1: // ipDiscoveryf
      sendAutenticationAck();
      break;
    case 5: // CMD_REQUEST_DATA
      teste();
      break;
    case 10:// new data sensro
      break;
    default:
      break;
  }
}


void teste() {
  Serial.println("Pacote de ipDiscovery recebido");
  char newSensorMac[] = {
    packetBuffer[3],
    packetBuffer[4],
    packetBuffer[5],
    packetBuffer[6],
    packetBuffer[7],
    packetBuffer[8]
  };
  addNewSensor(packetBuffer[2], newSensorMac);
}

void addNewSensor(int sensorType, byte mac[]) {
  Serial.println("Adicionando um novo sensor na lista ");
  int pos = getNextPossition();
  if (pos == -1) {
    Serial.println("Array está cheio");
    return;
  }
  sensorList[pos].typeData = sensorType;
  memcpy(sensorList[pos].mac, mac, 6);
  sensorList[pos].alive = true;
  Serial.print("Sensor Type: ");
  Serial.println(sensorList[pos].typeData);
  Serial.print("MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.println(sensorList[pos].mac[i], HEX);
  }
  Serial.print("POS: ");
  Serial.println(pos);
}


void requestData() {
  //  Serial.print("###TAMANHO DA LISTA" );
  //  Serial.println(sizeof(sensorList));
  for (int i = 0; i < MAX_SENSOR; i++) {
    if (sensorList[i].alive) {
      Serial.print("Requisitando dados ao sensor: ");
      Serial.print(i);
      Serial.print("alive: ");
      Serial.println( sensorList[i].alive);
      makePacketToGetData(i);
   
    }
  }
  delay(500);
}




