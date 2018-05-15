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
  if (packetBuffer[1] == CMD_DISCOVERY) {
    return 0;
  }

  if (packetBuffer[1] == CMD_DISCOVERY_REQUEST) {
    return 1;
  }
}


void makeIpOfferPacket() {

  Serial.println("MONTANDO PACOTE DE OFERTA DE IP: ");
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_DISCOVERY_OFFER;
  replayBuffer[2] = packetBuffer[2];
  replayBuffer[3] = packetBuffer[3];
  replayBuffer[4] = packetBuffer[4];
  replayBuffer[5] = packetBuffer[5];
  replayBuffer[6] = packetBuffer[6];
  replayBuffer[7] = packetBuffer[7];
  replayBuffer[8] = packetBuffer[8];
  replayBuffer[9] = ipOffer[0];
  replayBuffer[10] = ipOffer[1];
  replayBuffer[11] = ipOffer[2];
  replayBuffer[12] = ipOffer[3];
  replayBuffer[13] = EOT;

}

void makeIpAckPacket() {

  Serial.println("MONTANDO PACOTE DE ACK DE IP: ");
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_DISCOVERY_ACK;
  replayBuffer[2] = packetBuffer[2];
  replayBuffer[3] = packetBuffer[8];
  replayBuffer[4] = packetBuffer[7];
  replayBuffer[5] = packetBuffer[6];
  replayBuffer[6] = packetBuffer[5];
  replayBuffer[7] = packetBuffer[4];
  replayBuffer[8] = packetBuffer[3];
  replayBuffer[9] = ipOffer[0];
  replayBuffer[10] = ipOffer[1];
  replayBuffer[11] = ipOffer[2];
  replayBuffer[12] = ipOffer[3];
  replayBuffer[13] = EOT;
}

void makePacketToGetData(int sensor) {
  Serial.println("MONTANDO DE REQUISIÇÃO DE DADO A SENSOR ");

  replayBuffer[0] = SOH;
  replayBuffer[13] = EOT;
}

