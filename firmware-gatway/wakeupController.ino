/******************************************************************
   
    Arquivo os métodos de comunicação com o Módulo WakeUp
    
    @autor Jose Augusto Gorck
    @since 15/06/18
*/

void sendWakeupWord(int pos) {
  byte wpWord[5];
  for (int x = 0; x < 6; x++) {
    wpWord[x] = sensorList[pos]->mac[x];
  }
//  Serial.print("Palavra: ");
//  for (int x = 0; x < 6; x++) {
//    Serial.print(wpWord[x], HEX);
//  }
//  Serial.println();
  //teste
  sensorList[pos]->isAwake = true;
//
  vw_send((uint8_t *)wpWord, strlen(wpWord));
  vw_wait_tx(); // Wait until the whole message is gone

}
