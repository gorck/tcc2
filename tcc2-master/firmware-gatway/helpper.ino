/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/



int getCmdMessage() {
  if (packetBuffer[0] != SOH) {
    Serial.println("Mensagem recebida inválida");
    return -1;
  }
  Serial.println("Valor do byte 1");
  Serial.println(packetBuffer[1]);
  return packetBuffer[1];
}


//void makeIpOfferPacket() {
//
//  Serial.println("MONTANDO PACOTE DE OFERTA DE IP: ");
//  replayBuffer[0] = SOH;
//  replayBuffer[1] = CMD_DISCOVERY_OFFER;
//  replayBuffer[2] = packetBuffer[2];
//  replayBuffer[3] = packetBuffer[3];
//  replayBuffer[4] = packetBuffer[4];
//  replayBuffer[5] = packetBuffer[5];
//  replayBuffer[6] = packetBuffer[6];
//  replayBuffer[7] = packetBuffer[7];
//  replayBuffer[8] = packetBuffer[8];
//  replayBuffer[9] = ipOffer[0];
//  replayBuffer[10] = ipOffer[1];
//  replayBuffer[11] = ipOffer[2];
//  replayBuffer[12] = ipOffer[3];
//  replayBuffer[13] = EOT;
//
//}

void makeAutenticationAck() {

  Serial.println("MONTANDO PACOTE DE ACK: ");
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_DISCOVERY_ACK;
  replayBuffer[2] = packetBuffer[3];
  replayBuffer[3] = packetBuffer[4];
  replayBuffer[4] = packetBuffer[5];
  replayBuffer[5] = packetBuffer[6];
  replayBuffer[6] = packetBuffer[7];
  replayBuffer[7] = packetBuffer[8];
  replayBuffer[8] = packetBuffer[9];
  replayBuffer[9] = EOT;
}

void makePacketToGetData(int sensor) {
  Serial.println("MONTANDO PACOTE DE REQUISIÇÃO DE DADO A SENSOR ");
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_REQUEST_DATA;
  replayBuffer[2] = sensorList[sensor].mac[0];
  replayBuffer[3] = sensorList[sensor].mac[1];
  replayBuffer[4] = sensorList[sensor].mac[2];
  replayBuffer[5] = sensorList[sensor].mac[3];
  replayBuffer[6] = sensorList[sensor].mac[4];
  replayBuffer[7] = sensorList[sensor].mac[5];
  replayBuffer[8] = EOT;
  sendPacket();
}

void clearBuffer() {
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    replayBuffer[x] = 0x00;
  }
}
