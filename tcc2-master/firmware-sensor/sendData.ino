/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/

//
///**
//   Responde o servidor de autenticação
//*/
//void sendCmdDiscoveryRequestPacket() {
//  if (KDState.status_discovery != 2) {
//    Serial.println("Não pode ser enviado o pacote de Request...");
//    return;
//  }
//  Serial.println("Enviado Request do IP Discovery");
//  Serial.println(broadcast_ip);
//  Serial.println(discovery_port);
//  makeCmdDiscoveryRequestPacket();
//  for (int i = 0; i < DISCOVERY_PACKET_SIZE; i++) {
//    Serial.print(replayBuffer[i], HEX);
//  }
//  Serial.println(" ");
//  sendPacket();
//}

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

void sendDataSensor() {
  makeSensorResponsePacket();
  sendPacket();
}

void sendNodeStatus(int statusNode) {
  makeNodeStatussPacket(statusNode);
  sendPacket();
}

void sendPacket() {
  Serial.println("ENVIADO PACOTE....");
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    Serial.print(replayBuffer[x], HEX);
  }
  Serial.println();
  Serial.println(broadcast_ip);
  Serial.println(discovery_port);

  udp.begin(discovery_response_port);
  udp.beginPacket(broadcast_ip, discovery_port);
  udp.write(replayBuffer, BUFFER_PACKET_SIZE);
  udp.endPacket();
  Serial.println("PACOTE Enviado");
  Ethernet.maintain();
}










