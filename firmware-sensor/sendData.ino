/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/


/**
   Responde o servidor de autenticação
*/
void sendCmdDiscoveryRequestPacket() {
  if (KDState.statusDiscovery != 2) {
    Serial.println("Não pode ser enviado o pacote de Request...");
    return;
  }
  Serial.println("Enviado Request do IP Discovery");
  Serial.println(broadcastIP);
  Serial.println(discoveryPort);
  makeCmdDiscoveryRequestPacket();
  for (int i = 0; i < DISCOVERY_PACKET_SIZE; i++) {
    Serial.print(replayBuffer[i], HEX);
  }
  Serial.println(" ");
  sendPacket();
}

/**
   Envia uma mensagem em broadcast a procura do servidor de autenticação
*/
void discoveryGateway() {
  Serial.println("Buscando IP do Gateway da BAN:");
  makeCmdDiscoveryPacket();
  for (int i = 0; i < DISCOVERY_PACKET_SIZE; i++) {
    Serial.print(replayBuffer[i], HEX);
  }
  Serial.println(" ");
  sendPacket();
}




void sendPacket() {
  Serial.println("ENVIADO PACOTE....");
  Serial.println(broadcastIP);
  Serial.println(discoveryPort);

  udp.begin(discoveryResponsePort);
  udp.beginPacket(broadcastIP, discoveryPort);
  udp.write(replayBuffer, BUFFER_PACKET_SIZE);
  udp.endPacket();
  Serial.println("PACOTE Enviado");
  Ethernet.maintain();
}








