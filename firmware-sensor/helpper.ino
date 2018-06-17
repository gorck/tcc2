/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/


/**
   Método para capturar a ação a ser realizada pelo
   nodo de sensoriamento
*/
void wrapperAnswer() {
  Serial.println("RECEBENDO UMA NOVA MENSAGEM");
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    Serial.print(packetBuffer[x], HEX);
  }

  Serial.println();
  if (packetBuffer[0] != SOH) {
    Serial.println("Mensagem recebida inválida");
    return;
  }
  if (!checkAnswerIsEqualMac()) {
    Serial.println("Resposta de outro sensor. Endereço MAC de outro dispositivo");
    return;
  }

  //seta o mac do centralizador na memória
  setMacServer();

  if (CMD_DISCOVERY_ACK == packetBuffer[13]) {
    Serial.println("Autenticação recebida");
    Serial.println("Sensor pronto para operar");
    if (KDState.status_discovery != 4) {
      KDState.status_discovery = 1;
    } else {
      KDState.action = 0;
    }
  } else {
    Serial.print("### Requisição do gateway recbido: ");
    KDState.action = packetBuffer[13];
    Serial.println(KDState.action);
  }
}

/**
   Método para guardar na memória o MAC do centralizador

   Dado necessário para enviar a resposta de volta
*/
void setMacServer() {
  int offSet = 7;
  macServer[0] = packetBuffer[offSet];
  macServer[1] = packetBuffer[offSet + 1];
  macServer[2] = packetBuffer[offSet + 2];
  macServer[3] = packetBuffer[offSet + 3];
  macServer[4] = packetBuffer[offSet + 4];
  macServer[5] = packetBuffer[offSet + 5];
}

/**
   Método para montar o pacote de Dispovery
*/
void makeCmdDiscoveryPacket() {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = macServer[0];
  replayBuffer[2] = macServer[1];
  replayBuffer[3] = macServer[2];
  replayBuffer[4] = macServer[3];
  replayBuffer[5] = macServer[4];
  replayBuffer[6] = macServer[5];
  replayBuffer[7] = mac[0];
  replayBuffer[8] = mac[1];
  replayBuffer[9] = mac[2];
  replayBuffer[10] = mac[3];
  replayBuffer[11] = mac[4];
  replayBuffer[12] = mac[5];
  replayBuffer[13] = CMD_DISCOVERY;
  replayBuffer[14] = SENDOR_TYPE;
  replayBuffer[15] = 0x00;
  replayBuffer[16] = EOT;
}

/**
   Método para montar o pacote de resposta a requisição
   de um dado do sensor
*/
void makeSensorResponsePacket() {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = macServer[0];
  replayBuffer[2] = macServer[1];
  replayBuffer[3] = macServer[2];
  replayBuffer[4] = macServer[3];
  replayBuffer[5] = macServer[4];
  replayBuffer[6] = macServer[5];
  replayBuffer[7] = mac[0];
  replayBuffer[8] = mac[1];
  replayBuffer[9] = mac[2];
  replayBuffer[10] = mac[3];
  replayBuffer[11] = mac[4];
  replayBuffer[12] = mac[5];
  replayBuffer[13] = CMD_RESPONSE_NODE_DATA;
  replayBuffer[14] = SENDOR_TYPE;
  replayBuffer[15] = sizeof(KDState.last_data_sensor.bytes_data);
  replayBuffer[16] = KDState.last_data_sensor.bytes_data[0];
  replayBuffer[17] = KDState.last_data_sensor.bytes_data[1];
  replayBuffer[18] = KDState.last_data_sensor.bytes_data[2];
  replayBuffer[19] = KDState.last_data_sensor.bytes_data[3];
  replayBuffer[20] = EOT;

}

/**
   Método para montar o pacote de envio do status do nodo
   Caso ele esteja acordadno, é enviado no payload da mensagem
   o valor de um. Caso o nodo esteja indor dormir, será enviado
   o valor de zero;
*/
void makeNodeStatussPacket(int st) {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = macServer[0];
  replayBuffer[2] = macServer[1];
  replayBuffer[3] = macServer[2];
  replayBuffer[4] = macServer[3];
  replayBuffer[5] = macServer[4];
  replayBuffer[6] = macServer[5];
  replayBuffer[7] = mac[0];
  replayBuffer[8] = mac[1];
  replayBuffer[9] = mac[2];
  replayBuffer[10] = mac[3];
  replayBuffer[11] = mac[4];
  replayBuffer[12] = mac[5];
  replayBuffer[13] = CMD_RESPONSE_NODE_STATUS;
  replayBuffer[14] = SENDOR_TYPE;
  replayBuffer[15] = 0x01;
  replayBuffer[16] = st;
  replayBuffer[17] = EOT;

}

/**
   Método para verificar se o MAC recebido é do nodo
   Caso não seja, é descartada a mensagem
*/
bool checkAnswerIsEqualMac() {
  int offSet = 1;
  //  Serial.println("Autenticando MAC");
  //  Serial.println(packetBuffer[0 + offSet]);
  //  Serial.println(mac[0]);
  //  Serial.println(packetBuffer[1 + offSet]);
  //  Serial.println(mac[1]);
  return ( packetBuffer[0 + offSet] == mac[0] &&
           packetBuffer[1 + offSet] == mac[1] &&
           packetBuffer[2 + offSet] == mac[2] &&
           packetBuffer[3 + offSet] == mac[3] &&
           packetBuffer[4 + offSet] == mac[4] &&
           packetBuffer[5 + offSet] == mac[5]);
}

/**
   Método que limpa o buffer de envio. É setado todos os bytes em Zero
*/
void clearBuffer() {
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    replayBuffer[x] = 0x00;
  }
}

