
/**
   Método passando por parametro o tipo do sensor a ser requisitado,
   retornando a possição dele na struct.
*/
int findSensorByType(int type) {
  for (int x = 0; x < MAX_SENSOR; x++) {
    if (sensorList[x].typeData == type) {
      return x;
    }
  }
  return -1;
}

int getNextPossition() {
  for (int x = 0; x < MAX_SENSOR; x++) {
    if (sensorList[x].typeData == 0) {
      return x;
    }
  }
  return -1;
}

void getDataFromSensor(){

  
  sendPacket();
}

