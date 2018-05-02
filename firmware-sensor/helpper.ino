/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/



void wrapperAnswer() {
  if (packetBuffer[0] != SOH) {
    Serial.println("Mensagem recebida inválida");
    return;
  }

  if (KDState.statusDiscovery < 3) {
    if (!checkAnswerIsEqualMac()) {
      Serial.println("Resposta de outro sensor. Endereço MAC de outro dispositivo");
      return;
    }
  }
  if (packetBuffer[1] == CMD_DISCOVERY_OFFER) {
    Serial.println("Atribuindo novo endereço IP ao sensor");
    byte ip [] = {packetBuffer[9], packetBuffer[10], packetBuffer[11], packetBuffer[12]};
    restartEth(ip);
    KDState.statusDiscovery = 2;
  } else if (CMD_DISCOVERY_ACK) {
    Serial.println("Oferta de novo IP aceito");
    Serial.println("Sensor pronto para operar");
    KDState.statusDiscovery = 4;
  }
}



void makeCmdDiscoveryPacket() {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_DISCOVERY;
  replayBuffer[2] = SENDOR_TYPE;
  replayBuffer[3] = mac[0];
  replayBuffer[4] = mac[1];
  replayBuffer[5] = mac[2];
  replayBuffer[6] = mac[3];
  replayBuffer[7] = mac[4];
  replayBuffer[8] = mac[5];
  replayBuffer[9] = EOT;

}

void makeCmdDiscoveryRequestPacket() {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_DISCOVERY_REQUEST;
  replayBuffer[2] = SENDOR_TYPE;
  replayBuffer[3] = 0x01;
  replayBuffer[4] = EOT;

}



bool checkAnswerIsEqualMac() {

  int offSet = 3;
  /*
    for (int x = 0; x < 6; x++) {
    int i = x + offSet;
    if (data[i] != mac[x]) {
      Serial.print("Byte " );
      Serial.print( x );
      Serial.print(" não são iguais: ");
      Serial.print(data[i]);
      Serial.print(" x ");
      Serial.println(mac[x]);
    }
    }*/
  return ( packetBuffer[0 + offSet] == mac[0] &&
           packetBuffer[1 + offSet] == mac[1] &&
           packetBuffer[2 + offSet] == mac[2] &&
           packetBuffer[3 + offSet] == mac[3] &&
           packetBuffer[4 + offSet] == mac[4] &&
           packetBuffer[5 + offSet] == mac[5]);
}

void startEth(byte mac[], byte ip[]) {
  Ethernet.begin(mac, IPAddress(ip));
}

void restartEth(byte newIp[]) {
  Serial.print("Novo IP: ");
  ip[0] = newIp[0];
  ip[1] = newIp[1];
  ip[2] = newIp[2];
  ip[3] = newIp[3];
  Serial.print((int)ip[0]);
  Serial.print(".");
  Serial.print((int)ip[1]);
  Serial.print(".");
  Serial.print((int)ip[2]);
  Serial.print(".");
  Serial.print((int)ip[3]);
  //  Ethernet.begin(mac, IPAddress(ip));
  Serial.println("Reiniciando interface");
  Serial.println();
  //  Serial.println(Ethernet.maintain());
  //  Serial.println(Ethernet.localIP());
  //  udp.begin(discoveryPort);
}

void clearBuffer() {
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    replayBuffer[x] = 0x00;
  }
}

