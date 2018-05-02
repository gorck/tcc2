/******************************************************************

    @autor Jose Augusto Gorck
    @since 11/04/18

*/


void sendPacket() {
  Serial.println("ENVIADO PACOTE....");
  Serial.println(udp.remoteIP());
  Serial.println(udp.remotePort());

  udp.begin(discoveryPort);
  udp.beginPacket(udp.remoteIP(), udp.remotePort());
  udp.write(replayBuffer, BUFFER_PACKET_SIZE);
  udp.endPacket();
  Serial.println("PACOTE Enviado");
  Ethernet.maintain();
}

void sendIpAck() {

  makeIpAckPacket();
  Serial.println("ENVIADO ACK IP: ");
  for (int i = 0; i < BUFFER_PACKET_SIZE; i++) {
    Serial.print(replayBuffer[i], HEX);
  }
  Serial.print("  ");
  sendPacket();
}

void sendIpOffer() {
  makeIpOfferPacket();
  Serial.println("ENVIADO OFFER: ");
  for (int i = 0; i < BUFFER_PACKET_SIZE; i++) {
    Serial.print(replayBuffer[i], HEX);
  }
  Serial.print("  ");
  sendPacket();
}



