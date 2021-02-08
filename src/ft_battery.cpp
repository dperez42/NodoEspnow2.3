#include "main.hpp"

//---------------------------------------------------
// Read Battery
//---------------------------------------------------

float checkBattery(int battery_pin)
{ 
  adcStart(battery_pin);
  while(adcBusy(battery_pin));
  uint16_t sensorValue = adcEnd(battery_pin);
 float voltage = (((float)sensorValue/4096.0)*3.3)/(56.0/(56.0+47.0));
  return (voltage);
}
