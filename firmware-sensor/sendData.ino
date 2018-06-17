/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/

/**
   Envia uma mensagem em broadcast a procura do servidor de autenticação
*/
void discoveryGateway() {
  Serial.println("Buscando MAC do Gateway da BAN:");
  makeCmdDiscoveryPacket();
  Serial.println(" ");
  sendPacket();
}

/**
 * Método para montar o pacote de envio do dado do sensor
 * e enviar o pacote
 */
void sendDataSensor() {
  makeSensorResponsePacket();
  sendPacket();
}

/**
 * Método para montar o pacote de status do nodo e envia-lo
 */
void sendNodeStatus(int statusNode) {
  Serial.print("ENVIADO PACOTE DE STATUS DO NODO: ");
  Serial.println(statusNode);
  makeNodeStatussPacket(statusNode);
  sendPacket();
}

/*
 * Método implementado para enviar a mensagem na rede.
 */
void sendPacket() {

  Serial.println("ENVIADO PACOTE....");
  //  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
  //    Serial.print(replayBuffer[x], HEX);
  //  }

  int messageTotalSize = replayBuffer[15] + 17;
  byte messageBuffer[messageTotalSize];

  for (int x = 0; x < messageTotalSize; x++) {
    messageBuffer[x] = replayBuffer[x];
  }
  Serial.println();
  for (int x = 0; x < messageTotalSize; x++) {
    Serial.print(messageBuffer[x], HEX);
  }



  Serial.println();
  Serial.println(broadcast_ip);
  Serial.println(discovery_port);

  udp.begin(discovery_response_port);
  udp.beginPacket(broadcast_ip, discovery_port);
  udp.write(messageBuffer, messageTotalSize);
  udp.endPacket();
  Serial.println("PACOTE Enviado");
  Ethernet.maintain();

}










