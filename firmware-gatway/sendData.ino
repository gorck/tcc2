/******************************************************************

    Arquivo contendo os métodos de envio de comandos

    @autor Jose Augusto Gorck
    @since 11/04/18
*/


/**
   Método para enviar os dado em broadcast para a rede
*/
void sendPacket() {
  Serial.println("ENVIADO PACOTE....");
  int messageTotalSize = replayBuffer[15] + 17;
  byte messageBuffer[messageTotalSize];

  for (int x = 0; x < messageTotalSize; x++) {
    messageBuffer[x] = replayBuffer[x];
  }

  //  Serial.println();
  //  for (int x = 0; x < messageTotalSize; x++) {
  //    Serial.print(messageBuffer[x], HEX);
  //  }
  //  Serial.println();
  //  Serial.println(udp.remoteIP());
  //  //  Serial.println(udp.remotePort());
  //  Serial.println(broadcastIP);
  //  Serial.println(dataPort);

  udp.begin(discoveryPort);

  udp.beginPacket(broadcastIP, dataPort);
  //  udp.beginPacket(broadcastIP, udp.remotePort());
  //  udp.write(replayBuffer, BUFFER_PACKET_SIZE);
  udp.write(messageBuffer, messageTotalSize);
  udp.endPacket();
  Serial.println("PACOTE Enviado");
  Ethernet.maintain();
}

void sendPacketToGateway() {

  int messageTotalSize = replayBuffer[15] + 17;
  byte messageBuffer[messageTotalSize];

  for (int x = 0; x < messageTotalSize; x++) {
    messageBuffer[x] = replayBuffer[x];
  }
  //  Serial.println(udp.remoteIP());
  //  Serial.println(udp.remotePort());
  udp.begin(discoveryPort);
  udp.beginPacket(broadcastIP, udp.remotePort());
  udp.write(messageBuffer, messageTotalSize);
  udp.endPacket();
  Serial.println("PACOTE Enviado ao Gateway");
  Ethernet.maintain();
}

/**
   Metodo para monstar e enviar o pacote de autenticação de um nodo
*/

void sendConfigSensorTimesAck() {
  makeConfigSensorTimesAck();
  Serial.println("ENVIADO ACK OK GATEWAY: ");
  sendPacketToGateway();
}

void sendAutenticationAck() {
  makeAutenticationAck();
  Serial.println("ENVIADO ACK OK: ");
  sendPacket();
}

/**
   Método de envio de todos os dados do centralizador.
   Será montado um pacote contendo todos os dados dos sensores, data de aquisição
   e seus valores. Será enviada para uma porta especifica, parametrizada nas
   variáveis globais.

   Após o envio, é realizada o clear das informações já salvas.
*/
void getAllDataToGateway() {

  //  Serial.println("###PREPARANDO DADO A SER ENVIADO####");
  int messageTotalSize = ((sizeof(DataInfo) * MAX_DATA_SENSOR * MAX_SENSOR) + (MAX_SENSOR * 6));
  byte messageBuffer[messageTotalSize];

  memset(messageBuffer, 0, sizeof(messageBuffer));

  //Construção do Header da mensagem
  messageBuffer[0] = SOH;
  messageBuffer[1] = packetBuffer[7];
  messageBuffer[2] = packetBuffer[8];
  messageBuffer[3] = packetBuffer[9];
  messageBuffer[4] = packetBuffer[10];
  messageBuffer[5] = packetBuffer[11];
  messageBuffer[6] = packetBuffer[12];
  messageBuffer[7] = mac[0];
  messageBuffer[8] = mac[1];
  messageBuffer[9] = mac[2];
  messageBuffer[10] = mac[3];
  messageBuffer[11] = mac[4];
  messageBuffer[12] = mac[5];
  messageBuffer[13] = CMD_RESPONSE_DATA_LIST;
  messageBuffer[14] = SENDOR_TYPE;
  messageBuffer[15] = messageTotalSize;

  //  for (int i = 0; i < ((sizeof(DataInfo)*MAX_DATA_SENSOR * MAX_SENSOR) + MAX_SENSOR * 6); i++) {
  //    Serial.print(messageBuffer[i], HEX);
  //  }
  int offSet = 16; // offset do header da mensagem
  int offSetCycle = 46; //offset do sensor
  int offSetMac = 7; //offset de controle do mac
  int offSetDataInfo = 8; //offset de controle de cada mensagem enviada

  //construção do corpo da mensagem
  for (int i = 0; i < MAX_SENSOR; i++) {
    //    Serial.print("MAX_SENSOR =  ");
    //    Serial.println(i);
    //    Serial.print("MAC: ");
    for (int iMac = 0; iMac < 7; iMac ++) {
      messageBuffer[offSet + (i * offSetCycle) + iMac] = sensorList[i]->mac[iMac];
      //      Serial.print(messageBuffer[(i * offSet) + iMac], HEX);
    }
    //    Serial.println("");
    for (int pData = 0; pData < MAX_DATA_SENSOR; pData ++) {
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 0] = sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[0];
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 1] = sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[1];
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 2] = sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[2];
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 3] = sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[3];

      //      Serial.print("DATA timeAquisition: ");
      //      Serial.println(sensorList[i]->dataInfo[pData].timeAquisition.data);
      //      Serial.print("DATA timeAquisition BYTEs: ");
      //      Serial.print(sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[0], HEX);
      //      Serial.print(sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[1], HEX);;
      //      Serial.print(sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[2], HEX);
      //      Serial.println(sensorList[i]->dataInfo[pData].timeAquisition.bytes_data[3], HEX);

      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 4] = sensorList[i]->dataInfo[pData].info.bytes_data[0];
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 5] = sensorList[i]->dataInfo[pData].info.bytes_data[1];
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 6] = sensorList[i]->dataInfo[pData].info.bytes_data[2];
      messageBuffer[offSet + (i * offSetCycle) + offSetMac + (pData * offSetDataInfo) + 7] = sensorList[i]->dataInfo[pData].info.bytes_data[3];

      //      Serial.print("DATA info: ");
      //      Serial.println(sensorList[i]->dataInfo[pData].info.data);
      //      Serial.print("DATA info BYTEs: ");
      //      Serial.print(sensorList[i]->dataInfo[pData].info.bytes_data[0], HEX);
      //      Serial.print(sensorList[i]->dataInfo[pData].info.bytes_data[1], HEX);
      //      Serial.print(sensorList[i]->dataInfo[pData].info.bytes_data[2], HEX);
      //      Serial.println(sensorList[i]->dataInfo[pData].info.bytes_data[3], HEX);
    }
    //    for (int f = i * offSet; f < (i + 1) *offSet * 1; f++) {
    //      Serial.print(messageBuffer[f], HEX);
    //    }
    //    Serial.println();
  }

  sendPacketToGateway(messageBuffer, messageTotalSize);

  for (int i = 0; i < MAX_SENSOR; i++) {
    sensorList[i]->cleanData();
  }

}


void sendOndeDataToGatewat(byte dataRecived[]) {

  int messageTotalSize = (21);
  byte messageBuffer[messageTotalSize];

  memset(messageBuffer, 0, sizeof(messageBuffer));

  //Construção do Header da mensagem
  messageBuffer[0] = SOH;
  messageBuffer[1] = requesOnceData.macGateway[0];
  messageBuffer[2] = requesOnceData.macGateway[1];
  messageBuffer[3] = requesOnceData.macGateway[2];
  messageBuffer[4] = requesOnceData.macGateway[3];
  messageBuffer[5] = requesOnceData.macGateway[4];
  messageBuffer[6] = requesOnceData.macGateway[5];
  messageBuffer[7] = mac[0];
  messageBuffer[8] = mac[1];
  messageBuffer[9] = mac[2];
  messageBuffer[10] = mac[3];
  messageBuffer[11] = mac[4];
  messageBuffer[12] = mac[5];
  messageBuffer[13] = CMD_RESPONSE_DATA_LIST;
  messageBuffer[14] = SENDOR_TYPE;
  messageBuffer[15] = 0x04;
  messageBuffer[16] = dataRecived[0];
  messageBuffer[17] = dataRecived[1];
  messageBuffer[18] = dataRecived[2];
  messageBuffer[19] = dataRecived[3];
  messageBuffer[20] = EOT;

  sendPacketToGateway(messageBuffer, messageTotalSize);
  
  requesOnceData.dataReviced();
}




void sendPacketToGateway(byte *messageBuffer, int messageTotalSize) {

  udp.begin(discoveryPort);
  udp.beginPacket(broadcastIP, remPort);
  udp.write(messageBuffer, messageTotalSize);
  udp.endPacket();
  Serial.println("PACOTE Enviado ao Gateway");
  Ethernet.maintain();

  for (int i = 0; i < MAX_SENSOR; i++) {
    sensorList[i]->cleanData();
  }

}







