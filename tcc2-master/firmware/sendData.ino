/******************************************************************

    @autor Jose Augusto Gorck
    @since 11/04/18

*/


void sendMessageAlive(int port, byte ip[]) {

  char data[5];
  getSensorInfo(&data[0]);

  Serial.println("Enviando dados via UDP");
  int sus = Udp.beginPacket(ip, port);
  Serial.print("Pacote UDP monstado: " );
  Serial.println(sus);

  Udp.write(data);
  Serial.println("Enviando.....");
  Udp.endPacket();
  Serial.println("-------Enviando dados via UDP");

}

