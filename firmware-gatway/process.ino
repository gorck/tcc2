/******************************************************************

    Arquivo contendo os processamento dos comandos recebidos


    @autor Jose Augusto Gorck
    @since 11/05/18
*/
/**
    Método para processar o recebimento de um pacote de troca de
    status do nodo. É atualizado conforme o valor recebido no
    payload da mensagem, zero indor dormir e hum acordado.
*/
void  processNodeStatus() {
  for (int i = 0; i < MAX_SENSOR; i++) {
    if (isMacRecivedLikeThis(sensorList[i]->mac)) {
      //atualiza como  valor enviado no payload da mensagem
      sensorList[i]->isAwake = packetBuffer[16];
      //caso tenha uma solicitação de dado urgente
      if (requesOnceData.requesOnceDataPending) {
        if (compareToMac(sensorList[i]->mac, requesOnceData.mac) && sensorList[i]->isAwake) {
          makePacketToGetData(i);
        }
      }
      break;
    }
  }
}

/**
   Método para o processamento do pacote de configuração dos sensores a serem
   monitorados. Será limpo todos os dados dos sensore, MAC cadastrados, dados anteriores
   e adicionados a nova configuração na memoria do microcontrolador.
*/
void  processConfigPacket() {

  int positionSize = 14;
  int configSensorByteSize = 9;

  //  Serial.println("Limpando Lista de Sensores");
  //limpa a memóriasa
  //  memset(sensorList, 0, sizeof(sensorList));

  // busca o número de sensores recebidos pelo comando
  int numSensor = packetBuffer[positionSize] / configSensorByteSize;
  //  Serial.println("");
  //  Serial.print("Recebido a configuração de ");
  //  Serial.print(numSensor);
  //  Serial.println(" sensores");

  int offSetConfigSensor = 15 ;
  for (int x = 0; x < numSensor; x++) {
    //    Serial.print("ADICIONANDO SENSOR:  ");
    //    Serial.println(x);
    int refPosition = offSetConfigSensor + (x * configSensorByteSize);

    byte newSensorMac[] = {
      packetBuffer[refPosition],
      packetBuffer[refPosition + 1],
      packetBuffer[refPosition + 2],
      packetBuffer[refPosition + 3],
      packetBuffer[refPosition + 4],
      packetBuffer[refPosition + 5]
    };
    byte timeGetData[] = {
      packetBuffer[refPosition + 7],
      packetBuffer[refPosition + 8]
    };

    addNewSensorByConfig(packetBuffer[refPosition + 6], newSensorMac, timeGetData);
  }
  sendConfigSensorTimesAck();
}


/**
   Método para processar o recebimento de um pacote de dados
   Será buscado se na lista possui algum sensor com esse MAC
   e adicionado o valor a ele
*/
void processNodeDataInfo() {

  for (int i = 0; i < MAX_SENSOR; i++) {
    if (isMacRecivedLikeThis(sensorList[i]->mac)) {
      int refPosition = 16 ;
      char dataRecived[] = {
        packetBuffer[refPosition + 0],
        packetBuffer[refPosition + 1],
        packetBuffer[refPosition + 2],
        packetBuffer[refPosition + 3]
      };
      addDataOnPosition(i, dataRecived);
      sensorList[i]->lastTimeData = millis();

      for (int x = 0; x < 6; x++) {
        Serial.print(requesOnceData.mac[x], HEX);
      }
        Serial.print("requesOnceData.requesOnceDataPending: ");
        Serial.println(requesOnceData.requesOnceDataPending);
      for (int x = 0; x < 6; x++) {
        Serial.print(sensorList[i]->mac[x], HEX);
      }
      if (requesOnceData.requesOnceDataPending
          && compareToMac(sensorList[i]->mac, requesOnceData.mac)) {
      
        sendOndeDataToGatewat(dataRecived);
      }
    }
  }
}


/**
   Método que irá processar a solicitação de um dado especifico
   de um nodo.
*/
void processOneDataPacket() {

  int refPosition = 15;
  char dataRecived[] = {
    packetBuffer[refPosition + 0],
    packetBuffer[refPosition + 1],
    packetBuffer[refPosition + 2],
    packetBuffer[refPosition + 3],
    packetBuffer[refPosition + 4],
    packetBuffer[refPosition + 5]
  };

  int pos = isMacConfig(dataRecived);
  if (pos < 0) {
    return;
  }

  memcpy(requesOnceData.mac, dataRecived, 6);
  requesOnceData.requesOnceDataPending = true;

  if (sensorList[pos]->isAwake) {
    makePacketToGetData(pos);
  } else {
    sendWakeupWord(pos);
  }

}
