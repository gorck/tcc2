/******************************************************************
    @autor Jose Augusto Gorck
    @since 11/04/18
*/



int getCmdMessage() {
  if (packetBuffer[0] != SOH) {
    Serial.println("Mensagem recebida inválida");
    return -1;
  }
  Serial.println("Comando recebido");
  Serial.println(packetBuffer[13]);
  return packetBuffer[13];
}


//void makeIpOfferPacket() {
//
//  Serial.println("MONTANDO PACOTE DE OFERTA DE IP: ");
//  replayBuffer[0] = SOH;
//  replayBuffer[1] = CMD_DISCOVERY_OFFER;
//  replayBuffer[2] = packetBuffer[2];
//  replayBuffer[3] = packetBuffer[3];
//  replayBuffer[4] = packetBuffer[4];
//  replayBuffer[5] = packetBuffer[5];
//  replayBuffer[6] = packetBuffer[6];
//  replayBuffer[7] = packetBuffer[7];
//  replayBuffer[8] = packetBuffer[8];
//  replayBuffer[9] = ipOffer[0];
//  replayBuffer[10] = ipOffer[1];
//  replayBuffer[11] = ipOffer[2];
//  replayBuffer[12] = ipOffer[3];
//  replayBuffer[13] = EOT;
//
//}

void makeAutenticationAck() {

  Serial.println("MONTANDO PACOTE DE ACK: ");
  replayBuffer[0] = SOH;
  replayBuffer[1] = packetBuffer[7];
  replayBuffer[2] = packetBuffer[8];
  replayBuffer[3] = packetBuffer[9];
  replayBuffer[4] = packetBuffer[10];
  replayBuffer[5] = packetBuffer[11];
  replayBuffer[6] = packetBuffer[12];
  replayBuffer[7] = mac[0];
  replayBuffer[8] = mac[1];
  replayBuffer[9] = mac[2];
  replayBuffer[10] = mac[3];
  replayBuffer[11] = mac[4];
  replayBuffer[12] = mac[5];
  replayBuffer[13] = CMD_DISCOVERY_ACK;
  replayBuffer[14] = SENDOR_TYPE;
  replayBuffer[15] = 0x00;
  replayBuffer[16] = EOT;


}

void makePacketToGetData(int sensor) {
  Serial.println("MONTANDO PACOTE DE REQUISIÇÃO DE DADO A SENSOR ");
  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = sensorList[sensor].mac[0];
  replayBuffer[2] = sensorList[sensor].mac[1];
  replayBuffer[3] = sensorList[sensor].mac[2];
  replayBuffer[4] = sensorList[sensor].mac[3];
  replayBuffer[5] = sensorList[sensor].mac[4];
  replayBuffer[6] = sensorList[sensor].mac[5];
  replayBuffer[7] = mac[0];
  replayBuffer[8] = mac[1];
  replayBuffer[9] = mac[2];
  replayBuffer[10] = mac[3];
  replayBuffer[11] = mac[4];
  replayBuffer[12] = mac[5];
  replayBuffer[13] = CMD_REQUEST_NODE_DATA;
  replayBuffer[14] = SENDOR_TYPE;
  replayBuffer[15] = 0x00;
  replayBuffer[16] = EOT;
  sendPacket();
}

void clearBuffer() {
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    replayBuffer[x] = 0x00;
  }
}

void teste() {
  Serial.println("Pacote de ipDiscovery recebido");
  char newSensorMac[] = {
    packetBuffer[7],
    packetBuffer[8],
    packetBuffer[9],
    packetBuffer[10],
    packetBuffer[11],
    packetBuffer[12]
  };
  addNewSensor(packetBuffer[14], newSensorMac);
}


void  processNodeStatus() {
  bool findNode = false;
  for (int i = 0; i < MAX_SENSOR; i++) {
    if (findNodeByMac(sensorList[i].mac)) {
      Serial.println("######### Atualização do status do nodo recebido");
      sensorList[i].isAwake = packetBuffer[16];
      Serial.print("######### Novo Status: ");
      Serial.println(sensorList[i].isAwake);
      Serial.print("######### Posicao do sensor: ");
      Serial.println(i);
      findNode = true;
      break;
    }
  }

  if (!findNode) {
    Serial.println("######### Novo Sensor recebido");
    char newSensorMac[] = {
      packetBuffer[7],
      packetBuffer[8],
      packetBuffer[9],
      packetBuffer[10],
      packetBuffer[11],
      packetBuffer[12]
    };
    addNewSensor(packetBuffer[14], newSensorMac);
  }

}

void addNewSensor(int sensorType, byte mac[]) {
  Serial.println("Adicionando um novo sensor na lista ");
  int pos = getNextPossition();
  if (pos == -1) {
    Serial.println("Array está cheio");
    return;
  }
  sensorList[pos].typeData = sensorType;
  memcpy(sensorList[pos].mac, mac, 6);
  sensorList[pos].isAwake = true;
  Serial.print("Sensor Type: ");
  Serial.println(sensorList[pos].typeData);
  Serial.print("MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.println(sensorList[pos].mac[i], HEX);
  }
  Serial.print("POS: ");
  Serial.println(pos);
}

int findNodeByMac(byte macRecived[]) {

  Serial.print("MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(macRecived[i], HEX);
  }

  Serial.println();
  int offSet = 1;
  return (packetBuffer[0 + offSet] == macRecived[0] &&
          packetBuffer[1 + offSet] == macRecived[1] &&
          packetBuffer[2 + offSet] == macRecived[2] &&
          packetBuffer[3 + offSet] == macRecived[3] &&
          packetBuffer[4 + offSet] == macRecived[4] &&
          packetBuffer[5 + offSet] == macRecived[5]);


}



