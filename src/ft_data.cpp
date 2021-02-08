
#include "main.hpp"

//--------------------------------------------------------------------
//READ DATA SENSORS
//--------------------------------------------------------------------
void getData(DHTesp dht) {
    // Reading temperature for humidity takes about 250 milliseconds! Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    TempAndHumidity newValues = dht.getTempAndHumidity();
    // Check if any reads failed and exit early (to try again).
    if (dht.getStatus() != 0) {
      Serial.println("DHT11 error status: " + String(dht.getStatusString()));
      return;
    }
    myDataSensor.temp = (newValues.temperature-4.0)*10.0f;
    myDataSensor.hum = (newValues.humidity)*10.0f;
    // Reading from MQ131
    MQ131.setEnv(newValues.temperature,newValues.humidity);
    //MQ131.sample();
    myDataSensor.id = id_nodo;
    myDataSensor.DevType = Devtype;
    myDataSensor.VHard = VHard;
    myDataSensor.VSoft = VSoft;
    myDataSensor.o3 = (MQ131.getO3(PPB))*10.0f;
    myDataSensor.o3H = (MQ131.getO3(PPB))*10.0f;
    myDataSensor.bat = (checkBattery(BAT_PIN))*100.0f;
    myDataSensor.log = 1;
     if (myDataSensor.o3/10 > l1i){
      highState = true;
    } else {
      highState = false;
    }
    return;
}