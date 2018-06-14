/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
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

  if (CMD_DISCOVERY_ACK == packetBuffer[1]) {
    Serial.println("Autenticação recebida");
    Serial.println("Sensor pronto para operar");
    if (KDState.status_discovery != 4) {
      KDState.status_discovery = 1;
    } else {
      KDState.action = 0;
    }
  } else {
    Serial.print("### Requisição do gateway recbido: ");
    KDState.action = packetBuffer[1];
    Serial.println(KDState.action);
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

void makeSensorResponsePacket() {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_RESPONSE_DATA;
  replayBuffer[2] = SENDOR_TYPE;
  replayBuffer[3] = mac[0];
  replayBuffer[4] = mac[1];
  replayBuffer[5] = mac[2];
  replayBuffer[6] = mac[3];
  replayBuffer[7] = mac[4];
  replayBuffer[8] = mac[5];
  replayBuffer[9] = sizeof(KDState.last_data_sensor.bytes_data);
  replayBuffer[10] = KDState.last_data_sensor.bytes_data[0];
  replayBuffer[11] = KDState.last_data_sensor.bytes_data[1];
  replayBuffer[12] = KDState.last_data_sensor.bytes_data[2];
  replayBuffer[13] = KDState.last_data_sensor.bytes_data[3];
  replayBuffer[14] = EOT;

}


void makeNodeStatussPacket(int st) {
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = CMD_NODE_STATUS;
  replayBuffer[2] = SENDOR_TYPE;
  replayBuffer[3] = mac[0];
  replayBuffer[4] = mac[1];
  replayBuffer[5] = mac[2];
  replayBuffer[6] = mac[3];
  replayBuffer[7] = mac[4];
  replayBuffer[8] = mac[5];
  replayBuffer[9] = st;
  replayBuffer[10] = EOT;

}

bool checkAnswerIsEqualMac() {
  int offSet = 2;
  return ( packetBuffer[0 + offSet] == mac[0] &&
           packetBuffer[1 + offSet] == mac[1] &&
           packetBuffer[2 + offSet] == mac[2] &&
           packetBuffer[3 + offSet] == mac[3] &&
           packetBuffer[4 + offSet] == mac[4] &&
           packetBuffer[5 + offSet] == mac[5]);
}

void clearBuffer() {
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    replayBuffer[x] = 0x00;
  }
}

