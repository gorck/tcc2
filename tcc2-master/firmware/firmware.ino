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

#define SOH 0x01
#define STX 0x02
#define ETX 0x03
#define EOT 0x04
#define ETB 0x17
#define FS 0x1C
#define SOH 0x02

#define SENDOR_TYPE 0x01;

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

long lastUpdate = 0;

boolean messageAliveSended = false;

byte server[] = { 192, 168, 0, 111 };
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xAA};

unsigned int localPort = 1903;


char packetBuffer[UDP_TX_PACKET_MAX_SIZE];

EthernetUDP Udp;


//structure for handling KD current state
struct sensor {
  unsigned long dataConnect; //offset for time discretization
  byte mac[6];
  byte ip[4];
  int sensorType;
};

sensor sensorInfo;





void setup() {
  // start the Ethernet and UDP:
  Ethernet.begin(mac, IPAddress(192, 168, 0, 222));
  Serial.begin(9600);
  Udp.begin(6666);
}

void loop() {
  
  stateMachine();
  delay(10);
}

void stateMachine(char cmd) {

  switch (cmd) {//identify and apply the command
    case '0':
      break;
    case '1':
      break;
    case '2':
      break;
    case '3':
      break;
    case '4':
      break;
    case '5':
      break;
    case '6':
      break;
    case '7':
      break;
    default:
      break;
  }
}






