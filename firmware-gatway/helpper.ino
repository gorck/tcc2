/******************************************************************

    Arquivo contendo métodos de auxilo, makers de comandos


    @autor Jose Augusto Gorck
    @since 11/05/18
*/


/**
   Método para verificar se é mensagem enviada é endereçada ao Centralizador

   A mensagem será aceita se for o mesmo MAC do dispositivo, ou se for enviada
   contendo todos os bytes em zero
*/
bool checkAnswerIsEqualMac() {
  int offSet = 1;
  return ( packetBuffer[0 + offSet] == mac[0] &&
           packetBuffer[1 + offSet] == mac[1] &&
           packetBuffer[2 + offSet] == mac[2] &&
           packetBuffer[3 + offSet] == mac[3] &&
           packetBuffer[4 + offSet] == mac[4] &&
           packetBuffer[5 + offSet] == mac[5]);
}


/**
   Método que monta o pacote de autenticação ao sensor
*/
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

/**
   Método que monta o pacote de requisição a um sensor
*/
void makePacketToGetData(int sensor) {
  Serial.println("MONTANDO PACOTE DE REQUISIÇÃO DE DADO A SENSOR ");
  sensorList[sensor]->lastTimeRequested =  millis();

  clearBuffer();
  replayBuffer[0] = SOH;
  replayBuffer[1] = sensorList[sensor]->mac[0];
  replayBuffer[2] = sensorList[sensor]->mac[1];
  replayBuffer[3] = sensorList[sensor]->mac[2];
  replayBuffer[4] = sensorList[sensor]->mac[3];
  replayBuffer[5] = sensorList[sensor]->mac[4];
  replayBuffer[6] = sensorList[sensor]->mac[5];
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

void makeConfigSensorTimesAck() {

  Serial.println("MONTANDO PACOTE DE ACK GATEWAY: ");
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
  replayBuffer[13] = CMD_CONFIG_SENSOR_TIMES_ACK;
  replayBuffer[14] = SENDOR_TYPE;
  replayBuffer[15] = 0x00;
  replayBuffer[16] = EOT;
}

/**
   Método que limpa o buffer de envio. É setado todos os
   bytes em Zero
*/
void clearBuffer() {
  for (int x = 0; x < BUFFER_PACKET_SIZE; x++) {
    replayBuffer[x] = 0x00;
  }
}


/**
   Método que adiciona um novo sensor a lista dos sensores do centralizado
*/
void addNewSensor(int sensorType, byte mac[]) {
  //  Serial.println("Adicionando um novo sensor na lista ");
  int pos = getNextPossition();
  if (pos == -1) {
    Serial.println("Array está cheio");
    return;
  }
  sensorList[pos]->typeData = sensorType;
  memcpy(sensorList[pos]->mac, mac, 6);
  sensorList[pos]->isAwake = true;
  Serial.print("Sensor Type: ");
  Serial.println(sensorList[pos]->typeData);
}

/**
   Método para buscar lista dos sensores pelo MAC do dispositivo.
   Irá percorrer a lista comparando os MAC e retornando a possição
   da lista que ele estiver
*/

boolean isMacRecivedLikeThis(byte macRecived[]) {
  int offSet = 7;
  return (packetBuffer[0 + offSet] == macRecived[0] &&
          packetBuffer[1 + offSet] == macRecived[1] &&
          packetBuffer[2 + offSet] == macRecived[2] &&
          packetBuffer[3 + offSet] == macRecived[3] &&
          packetBuffer[4 + offSet] == macRecived[4] &&
          packetBuffer[5 + offSet] == macRecived[5]);
}



/**
   Método que adiciona um novo sensor recebido pelas configuração a lista dos
   sensores do Centralizador
*/
void addNewSensorByConfig(int sensorType, byte *mac, byte *timeToGetData) {
  //  Serial.println("## Adicionando um novo sensor na lista ");
  int pos = getNextPossition();
  if (pos == -1) {
    Serial.println("Array está cheio");
    return;
  }
  sensorList[pos]->typeData = sensorType;
  //copia o mac do dispositivo
  memcpy(sensorList[pos]->mac, mac, 6);
  sensorList[pos]->isAwake = false;
  memcpy(sensorList[pos]->timeToGetData.bytes_data, timeToGetData, 2);

  //  Serial.print("Sensor Type: ");
  //  Serial.println(sensorList[pos]->typeData);
  //  Serial.print("Tempo de aquisição: ");
  //  Serial.println(sensorList[pos]->timeToGetData.data);
}

/**
   Adiciona o valor recebido pelo sensor na posição correta na memória
*/
void addDataOnPosition(int i, char dataRecived[]) {
  for (int x = 0; x <= MAX_DATA_SENSOR; x++) {
    //testa a posição da memória que ainda não foi usada
    if (sensorList[i]->dataInfo[x].timeAquisition.data == 0) {
      //copia o dado para a memória
      memcpy(sensorList[i]->dataInfo[x].info.bytes_data, dataRecived, 4);
      sensorList[i]->dataInfo[x].timeAquisition.data = millis();
      break;
    }
  }
}




/**
   Método que verifica se um MAC, passando como referencia para
   a função, está na lista de sensores cadastrados.

   Caso esteja cadastrado, irá retornar com o aposição da lista,
   caso não possua nenhum, será retornado com posição -1.
*/
int isMacConfig(char dataRecived[]) {

  for (int i = 0; i < MAX_SENSOR; i++) {
    if (compareToMac(sensorList[i]->mac, dataRecived)) {
      return i;
    }
  }
  return -1;
}

/**
   Método para comparar de os dois MACs enviado por parametro
   são igual
*/
boolean compareToMac(byte dataRecived[], byte dataRecived1[]) {
  return (
           dataRecived1[0] == dataRecived[0] &&
           dataRecived1[1] == dataRecived[1] &&
           dataRecived1[2] == dataRecived[2] &&
           dataRecived1[3] == dataRecived[3] &&
           dataRecived1[4] == dataRecived[4] &&
           dataRecived1[5] == dataRecived[5]
           );
}

