/******************************************************************

    Arquivo contendo os métodos de requisição de dado

    @autor Jose Augusto Gorck
    @since 10/06/18
*/


/**
   Método que busca na mensagem recebida o ação a ser executada pelo Centralizador
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


/**
   Método que percorre todos os sensores configurados. Neste método será realizado a
   consulta se é necessário a aquisição de um dado de um determinado sensor. Caso seja
   necessário, é realizado a consulta da flag de acordado do sensor. Caso o nodo esteja
   acordado, é realizado o envio do pacote de solicitação, caso esteja dormindo, é enviado
   a palavra de acordamento ao módulo de wakeup
*/
void requestData() {
  for (int i = 0; i < MAX_SENSOR; i++) {
    if (isTimeTOGetData(i)) {
      if (sensorList[i]->isAwake) {
        makePacketToGetData(i);
      } else {
        //        Serial.println("Enviando pacote ao Wakeup a palavra de acordamento: ");
        sendWakeupWord(i);
      }
    }
  }
}


/**
   Método passando por parametro o tipo do sensor a ser requisitado,
   retornando a possição dele na struct.
*/
int findSensorByType(int type) {
  for (int x = 0; x < MAX_SENSOR; x++) {
    if (sensorList[x]->typeData == type) {
      return x;
    }
  }
  //caso não achar, envia -1
  return -1;
}

/**
   Busca a próxima possição a ser adicionado um sensor na memória
*/
int getNextPossition() {
  for (int x = 0; x < MAX_SENSOR; x++) {
    Serial.print("Sensor Type: ");
    Serial.println(sensorList[x]->typeData);
    if (sensorList[x]->typeData == 0) {
      return x;
    }
  }
  return -1;
}


/**
   Método para veridicar se está no momento de escalonar uma requisição
   de dado ao sensor. É realizada a comparação com o tempo corrente do
   microcontrolador conforme a configuração recebida pelo gateway. Caso
   seja necessário, irá retornar com TRUE, caso contrário, FALSE.
*/
boolean isTimeTOGetData(int i) {
  if (sensorList[i]->timeToGetData.data == 0) {
    return false;
  }

  if (sensorList[i]->timeToGetData.data * 1000 < millis() - sensorList[i]->lastTimeData) {
    if (sensorList[i]->lastTimeRequested == 0) {
      sensorList[i]->lastTimeRequested =  millis();
      return true;
    }
    //filtro para não enviar muitos pacotes de requisição
    //if para aguardar entre um requisição e o seu reenvio
    if (sensorList[i]->lastTimeRequested >= (  millis() - 500)) {
      return false;
    }
    //    Serial.print("Precisa capturar dado do sensor: ");
    //    Serial.println(i);
    sensorList[i]->lastTimeRequested =  millis();
    return true;
  }
  return false;
}






