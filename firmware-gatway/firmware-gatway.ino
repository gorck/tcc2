/*******************************************************************

    Classe principal contendo os imports e logica combinacional

    @autor Jose Augusto Gorck
    @since 23/03/18

*/

/********************** LIBS *********************/

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <VirtualWire.h>



/********************** DEFINE VARIABLES *********************/

#define SERIAL_BAUD_RATE 9600       //Serial baud rate 

#define SOH 0x01 //start of transmission
#define STX 0x02 //start of text
//#define ETX 0x03 //end of text
#define EOT 0x04 //end of transmission
//#define ETB 0x17 //end od trans block
//#define FS 0x1C //file separator


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


#define CMD_CONFIG_SENSOR_TIMES 0x0A // 
#define CMD_CONFIG_SENSOR_TIMES_ACK 0x0B // 
#define CMD_REQUEST_DATA_LIST 0x0C // 
#define CMD_RESPONSE_DATA_LIST 0x0D // 
#define CMD_REQUEST_ONE_DATA 0x0E // 
#define CMD_RESPONSE_ONE_DATA 0x0F // 

#define CMD_LIST_SENSOR 0x0B // Lista de sensores recebida pelo gateway
#define CMD_DATA 0x0A // novo dado

#define SENDOR_TYPE  99 // tipo de sensor, neste caso é o centralizador

#define MAX_DATA_SENSOR 5
#define MAX_SENSOR 5


/********************** GLOBAL VARIABLES *********************/
struct DataInfo {
  //  unsigned long timeAquisition = 0;
  union {
    long data;
    byte bytes_data[4];
  } timeAquisition;

  union {
    float data;
    unsigned char bytes_data[4];
  } info;
};


struct Sensor {
  //unsigned long timeToGetData = 0;
  union {
    long data;
    byte bytes_data[4];
  } timeToGetData;

  union {
    float data;
    unsigned char bytes_data[4];
  } info;

  unsigned long lastTimeData = 0;
  unsigned long lastTimeRequested = 0;
  int typeData = 0;
  byte mac[6];
  bool isAwake = false;
  DataInfo dataInfo [MAX_DATA_SENSOR];

  void cleanData() {
    lastTimeData = 0;
    lastTimeRequested = 0;
    timeToGetData.data = 0;
    info.data = 0;
    memset(dataInfo, 0, sizeof(dataInfo));
  }
  void reset() {
    isAwake = false;
    typeData = 0;
    memset(mac, 0, sizeof(mac));
    cleanData();
  }
} sensor;


struct RequesOnceData {
  byte mac[6];
  byte macGateway[6];
  boolean requesOnceDataPending = false;
  void reset() {
    dataReviced();
  }
  void dataReviced() {
    requesOnceDataPending = false;
    memset(mac, 0, sizeof(mac));
  }
} requesOnceData;


byte server[] = { 192, 168, 0, 111 };
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA};

const int BUFFER_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[BUFFER_PACKET_SIZE];
byte replayBuffer[BUFFER_PACKET_SIZE];

//Sensor **sensorList;
//int sensorCount = 0;
//Sensor s;
Sensor *(sensorList[MAX_SENSOR]);

//unsigned int localPort = 1903;
unsigned int remPort = 9876;

unsigned int discoveryPort = 6667;

unsigned int dataPort = 6666;

EthernetUDP udp; // discoveryServer

IPAddress broadcastIP(255, 255, 255, 255);

/**
   Configurações de statur do microcontrolador
*/
void setup() {
  Serial.begin(9600);
  // start the Ethernet and UDP:
  Serial.println("START UP Centralizador");
  Ethernet.begin(mac, IPAddress(192, 168, 0, 222));

  //  Serial.print("Iniciando UDP na porta:");
  //  Serial.println(discoveryPort);
  //Inicia o UDP
  udp.begin(discoveryPort);

  //Aloca a memória para os sensores
  for (int i = 0; i < MAX_SENSOR; i++) {
    sensorList[i] = (Sensor *)malloc(sizeof(Sensor));
  }

  //limpa todas as variaveis da struct
  for (int i = 0; i < MAX_SENSOR; i++) {
    sensorList[i]->reset();
  }

  //controle de requisição
  sensor.reset();

  //inicia a biblioteca do Wire, comunicação com o Módulo WakeUP
  /*******************************************
              Não usar pino 11 e 12
      Esses pinos são o padrão da shild ETH
   *******************************************/
  vw_set_ptt_inverted(true);
  vw_set_tx_pin(6);
  vw_set_ptt_pin(7);
  vw_set_rx_pin(8);
  vw_setup(4000);// speed of data transfer Kbps

  Serial.println("START UP Finalizado");
}

/**
   Loop principal do programa, verifica se possui alguma mensagem,
   caso tenha, realiza o parse dela e verifica o seu comando.

   A cada loop do ciclo é verificado se não é necessário o escalonamento
   de uma requisição de dado a algum sensor.
*/
void loop() {
  udp.begin(discoveryPort);
  if (udp.parsePacket()) {
    newPacketRecived();
  }
  requestData();
}

/**
   Método responsavel em envocar o comando conforme o pacote recebido
*/
void newPacketRecived() {
  //  Serial.println("Novo Pacote........");
  udp.read(packetBuffer, BUFFER_PACKET_SIZE);

  //  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
  //    Serial.print(packetBuffer[x], HEX);
  //  }
  Serial.println();
  int action = getCmdMessage();
  Serial.print("###### RECEBIDO ACTION: ");
  Serial.println(action);

  switch (action) {            //identify and apply the command
    case 0: // ipDiscovery
      //do nothing
      break;
    case CMD_DISCOVERY: // ipDiscoveryf
      sendAutenticationAck();
      break;
    case CMD_RESPONSE_NODE_DATA: // Comando de alteração do status de algum nodo
      processNodeDataInfo();
      break;
    case CMD_RESPONSE_NODE_STATUS: // Comando de alteração do status de algum nodo
      processNodeStatus();
      break;
    case CMD_REQUEST_DATA_LIST: //Comando de requisição de todos os dados do sensor
      sendAllDataToGateway();
      break;
    case CMD_CONFIG_SENSOR_TIMES: //Comando de configuração de tempos de aquisição dos sensores
      processConfigPacket();
      break;
    case CMD_REQUEST_ONE_DATA: //Comando de configuração de tempos de aquisição dos sensores
      processOneDataPacket();
      break;
    default:
      break;
  }
}







