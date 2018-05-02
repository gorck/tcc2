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

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>


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

//class Sensor {
//  public:
//    //    Sensor();
//    unsigned long millisOffset; //offset for time discretization
//    byte mac[6];
//    byte ip[4];
//    bool conected;
//    unsigned int remPort;
//    unsigned int sensorType;
//};


byte server[] = { 192, 168, 0, 111 };
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA};

byte ipOffer[] = { 192, 168, 0, 11};
byte ipOffer12[] = { 192, 168, 0, 12};
byte ipOffer13[] = { 192, 168, 0, 13};


const int BUFFER_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[BUFFER_PACKET_SIZE];
byte replayBuffer[BUFFER_PACKET_SIZE];


//unsigned int localPort = 1903;
unsigned int remPort = 5544;

unsigned int discoveryPort = 6667;

unsigned int dataPort = 666;

//const int maxSensor = 5;

//Sensor sensor1, sensor2, sensor3, sensor4, sensor5;
//Sensor sensorList[maxSensor] {sensor1, sensor2, sensor3, sensor4, sensor5};

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
  printAllSensorConected();
  delay(500);
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

/*
  void responseNewSensor(EthernetUDP udp) {

  Serial.print("RECEBIDO DE: IP ");
  Serial.print(udp.remoteIP());
  Serial.print(" - porta: ");
  Serial.println(udp.remotePort());
  Serial.println("DADOS DO NOVO SENSOR: ");
  Serial.print("TAMANHO DO PACOTE: ");
  Serial.println(UDP_TX_PACKET_MAX_SIZE);

  char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
  udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
  Serial.println(packetBuffer, HEX);

  }
*/

void addNewSensor() {
  //  Serial.println("Adicionando um novo sensor na lista ");
  //  Sensor sensor = Sensor();
  //  sensor.ip[4] = udp.remoteIP() ;
  //  sensor.conected = true;
  //  sensor.remPort = udp.remotePort();
  //  sensor.sensorType = 1;
  //  sensorList[0] = sensor;
}

int printAllSensorConected() {
  int totalConected = 0;
  //  for (int i = 0 ; i < maxSensor ; i++) {
  //    if (sensorList[i].conected) {
  //      totalConected++;
  //    }
  //  }
  Serial.print("Total de sensores conectados: ");
  Serial.println(totalConected);
  return totalConected;
}




