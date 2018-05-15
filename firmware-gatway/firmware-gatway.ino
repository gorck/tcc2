/*******************************************************************

    Classe principal contendo os imports e logica combinacional

    @autor Jose Augusto Gorck
    @since 23/03/18

*/

/********************** LIBS *********************/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>



/********************** DEFINE VARIABLES *********************/

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
  IPAddress ip;
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

}

void loop() {
  udp.begin(discoveryPort);
  if (udp.parsePacket()) {
    Serial.println("Recebido novo pacote ");
    newPacketRecived();
  }
}

void newPacketRecived() {
  Serial.println("Novo Pacote........");
  udp.read(packetBuffer, BUFFER_PACKET_SIZE);

  int action = getCmdMessage();

  switch (action) {            //identify and apply the command
    case 0: // ipDiscovery
      Serial.println("Pacote de ipDiscovery recebido");
      newSensorIP = udp.remoteIP();
      sendIpOffer();
      break;
    case 1: // ipDiscoveryf
      sendIpAck();
      break;
    case 10:// new data sensro
      break;
  }
}


void addNewSensor(long timeToGetData, int sensorType) {
  Serial.println("Adicionando um novo sensor na lista ");
  int pos = getNextPossition();
  if (pos == -1) {
    Serial.println("Arrai está cheio");
    return;
  }
  sensorList[pos].timeToGetData = timeToGetData;
  sensorList[pos].typeData = sensorType;
  sensorList[pos].ip = IPAddress(udp.remoteIP());
}




