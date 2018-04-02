/*******************************************************************
 *  
 *  Classe principal contendo os imports e logica combinacional
 *  
 *  @autor Jose Augusto Gorck
 *  @since 23/03/18
 *  
 */

/***************** LIST OF PINS BEING USED ******************* 
 * A0  - 
 * A1  - 
 * D0  - Bluetooth/RX
 * D1  - Bluetooth/TX
 * D4  - 
 */
 
 
 /********************** GLOBAL VARIABLES *********************/
#define SERIAL_BAUD_RATE 9600       //Serial baud rate 


#include <Wire.h>

  
void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
}

void loop() {  
  
  delay(100);  

}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
