/******************************************************************

    @autor Jose Augusto Gorck
    @since 11/04/18

*/


void sendPacket() {
  Serial.println("ENVIADO PACOTE....");
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    Serial.print(replayBuffer[x], HEX);
  }

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
  Serial.println(udp.remoteIP());
  Serial.println(udp.remotePort());

  udp.begin(discoveryPort);
  udp.beginPacket(broadcastIP, udp.remotePort());
  //  udp.write(replayBuffer, BUFFER_PACKET_SIZE);
  udp.write(messageBuffer, messageTotalSize);
  udp.endPacket();
  Serial.println("PACOTE Enviado");
  Ethernet.maintain();
}

void sendAutenticationAck() {
  makeAutenticationAck();
  Serial.println("ENVIADO ACK OK: ");
  sendPacket();
}
//
//void sendIpOffer() {
//  makeIpOfferPacket();
//  Serial.println("ENVIADO OFFER: ");
//  for (int i = 0; i < BUFFER_PACKET_SIZE; i++) {
//    Serial.print(replayBuffer[i], HEX);
//  }
//  Serial.print("  ");
//  sendPacket();
//}



