#include "MQ131.h"

/**
 * Constructor, nothing special to do
 */
MQ131Class::MQ131Class(uint32_t _RL) {
  valueRL = _RL;
}

/**
 * Destructor, nothing special to do
 */
MQ131Class::~MQ131Class() {
}

/**
 * Init core variables
 */
 void MQ131Class::begin(uint8_t _pinSensor, MQ131Model _model, uint32_t _RL) { 
 
   // Setup the model
  model = _model;

  // Store the circuit info (pin and load resistance)
  pinSensor = _pinSensor;
  valueRL = _RL;


  // Setup default calibration value
  switch(model) {
    case LOW_CONCENTRATION :
      setR0(MQ131_DEFAULT_LO_CONCENTRATION_R0);
      break;
    case HIGH_CONCENTRATION :
      setR0(MQ131_DEFAULT_HI_CONCENTRATION_R0);
      break; 
  }

  // Setup pin mode
  pinMode(pinSensor, INPUT);
}

/**
 * Do a full cycle (heater, reading, stop heater)
 * The function gives back the hand only at the end
 * of the read cycle!
 */
 void MQ131Class::sample() {
  lastValueRs = readRs();
 }

/**
 * Read Rs value
 */
 float MQ131Class::readRs() {
  adcStart(pinSensor);
  while(adcBusy(pinSensor));
  //Serial.println(adcEnd(pinAD));
  uint16_t valueSensor = adcEnd(pinSensor);
  //uint16_t valueSensor = analogRead(pinSensor);
  //valueSensor = (int)ADC_LUT[valueSensor];  // Correct the value
  /*
  uint16_t valueSensor = 0;
  for (int i = 0; i < 64; i++) {
        valueSensor += adc1_get_raw(ADC1_CHANNEL_5);
        }
  valueSensor /= 64;
  */
  
  
  
  // Compute the voltage on load resistance (for 3.3v Esp32)
  float vRL = ((float)valueSensor) / 4096.0 * 3.32;
  /*
  Serial.print(F("Pin Read:"));
  Serial.println(valueSensor);
  Serial.print(F("Voltaje:"));
  Serial.println(vRL);
  */
  // Compute the resistance of the sensor (for 3.3v Esp32)
  if(!vRL) return 0.0f; //division by zero prevention
  float rS = (3.3 / vRL - 1.0) * valueRL;
  /*
  Serial.print(F("Resistencia:"));
  Serial.println(rS);
  */
  return rS;
 }

/**
 * Set environmental values
 */
 void MQ131Class::setEnv(int8_t tempCels, uint8_t humPc) {
  temperatureCelsuis = tempCels;
  humidityPercent = humPc;
 }

/**
 * Get correction to apply on Rs depending on environmental
 * conditions
 */
 float MQ131Class::getEnvCorrectRatio() {
  // Select the right equation based on humidity
  // If default value, ignore correction ratio
  if(humidityPercent == 60 && temperatureCelsuis == 20) {
    return 1.0;
  }
  // For humidity > 75%, use the 85% curve
  if(humidityPercent > 75) {
    // R^2 = 0.9986
    return -0.0141 * temperatureCelsuis + 1.5623;
  }
  // For humidity > 50%, use the 60% curve
  if(humidityPercent > 50) {
    // R^2 = 0.9976
    return -0.0119 * temperatureCelsuis + 1.3261;
  }

  // Humidity < 50%, use the 30% curve
  // R^2 = 0.996
  return -0.0103 * temperatureCelsuis + 1.1507;
 }

 /**
 * Get gas concentration for O3 in ppm
 */
 float MQ131Class::getO3(MQ131Unit unit) {
  lastValueRs = readRs();
  // If no value Rs read, return 0.0
  if(lastValueRs < 0) {
    return 0.0;
  }

  float ratio = 0.0;

  switch(model) {
    case LOW_CONCENTRATION :
      // Use the equation to compute the O3 concentration in ppm
      // Compute the ratio Rs/R0 and apply the environmental correction
      ratio = lastValueRs / valueR0 * getEnvCorrectRatio();
      
      // R^2 = 0.9906
      // Use this if you are monitoring low concentration of O3 (air quality project)
      //return convert(9.4783 * pow(ratio, 2.3348), PPB, unit);
      //return convert(pow(10,((log10(1/ratio)-0.30103)/0.5 + 1.69897)), PPB, unit);
      //return convert(pow(10,((log10(ratio)-0.04557)/-0.366726+ 1.69897)), PPB, unit);
      //return convert(pow(10,((log10(ratio)-0)/-0.261439 + 1)), PPB, unit);
      return convert(pow(10,((log10(1/ratio)-(-0.4393))/0.4443)), PPB, unit);
      // R^2 = 0.9986 but nearly impossible to have 0ppb
      // Use this if you are constantly monitoring high concentration of O3
      // return convert((10.66435681 * pow(ratio, 2.25889394) - 10.66435681), PPB, unit);

    case HIGH_CONCENTRATION :
      // Use the equation to compute the O3 concentration in ppm
      
      // Compute the ratio Rs/R0 and apply the environmental correction
      ratio = lastValueRs / valueR0 * getEnvCorrectRatio();
      // R^2 = 0.9900
      // Use this if you are monitoring low concentration of O3 (air quality project)
      return convert(8.1399 * pow(ratio, 2.3297), PPM, unit);
      
      // R^2 = 0.9985 but nearly impossible to have 0ppm
      // Use this if you are constantly monitoring high concentration of O3
      // return convert((8.37768358 * pow(ratio, 2.30375446) - 8.37768358), PPM, unit);

      break;
      
    default :
      return 0.0;
  }
}

 /**
  * Convert gas unit of gas concentration
  */
 float MQ131Class::convert(float input, MQ131Unit unitIn, MQ131Unit unitOut) {
  if(unitIn == unitOut) {
    return input;
  }

  float concentration = 0;

  switch(unitOut) {
    case PPM :
      // We assume that the unit IN is PPB as the sensor provide only in PPB and PPM
      // depending on the type of sensor (METAL or BLACK_BAKELITE)
      // So, convert PPB to PPM
      return input / 1000.0;
    case PPB :
      // We assume that the unit IN is PPM as the sensor provide only in PPB and PPM
      // depending on the type of sensor (METAL or BLACK_BAKELITE)
      // So, convert PPM to PPB
      return input * 1000.0;
    case MG_M3 :
      if(unitIn == PPM) {
        concentration = input;
      } else {
        concentration = input / 1000.0;
      }
      return concentration * 48.0 / 22.71108;
    case UG_M3 :
      if(unitIn == PPB) {
        concentration = input;
      } else {
        concentration = input * 1000.0;
      }
      return concentration * 48.0 / 22.71108;
    default :
      return input;
  }
}

 /**
  * Calibrate the basic values (R0 and time to read)
  */
void MQ131Class::calibrate() {
  unsigned long int preheat;
  // Take care of the last Rs value read on the sensor
  // (forget the decimals)
  float lastRsValue = 0;
  //float lastLastRsValue = 0;
  // Count how many time we keep the same Rs value in a row
  //uint8_t countReadInRow = 0;
  // Count how long we have to wait to have consistent value
  //uint8_t count = 0;

  // Get some info
    Serial.println(F("MQ131 : Starting calibration..."));
    Serial.println(F("MQ131 : Enable heater"));
    Serial.print(F("MQ131 : Stable cycles required : "));
    Serial.print(MQ131_DEFAULT_STABLE_CYCLE);
    Serial.println(F(" (compilation parameter MQ131_DEFAULT_STABLE_CYCLE)"));

  //uint8_t timeToReadConsistency = MQ131_DEFAULT_STABLE_CYCLE;
    
    //PREHEAT SENSOR 15 SG
    Serial.println("Heating sensor! 15sg");
    preheat = millis();
    while ((millis()-preheat)<15000){
      Serial.print(".");
      delay(1000);
    } 
   /*
  while(countReadInRow <= timeToReadConsistency) {
    float value = readRs();
      Serial.print(F("MQ131 : Rs read = "));
      Serial.print((uint32_t)value);
      Serial.println(F(" Ohms"));
    
    if((uint32_t)lastRsValue != (uint32_t)value && (uint32_t)lastLastRsValue != (uint32_t)value) {
      lastLastRsValue = lastRsValue;
      lastRsValue = value;
      countReadInRow = 0;
    } else {
      countReadInRow++;
    }
    Serial.print(countReadInRow);
    Serial.print(" de ");
    Serial.println(count);
    count++;
    delay(1000);
  }

    Serial.print(F("MQ131 : Stabilisation after "));
    Serial.print(count);
    Serial.println(F(" seconds"));
*/
    float rs = 0;
    for (int i = 0;i < 50;i++) {
      rs += readRs();
      delay(200);
    }
    lastRsValue = rs / 50;
    lastRsValue = lastRsValue / 1.1727688; //Suponemos concentracion normal 10ppb
    // We have our R0 and our time to read
    setR0(lastRsValue);
}

 /**
  * Store R0 value (come from calibration or set by user)
  */
  void MQ131Class::setR0(float _valueR0) {
    valueR0 = _valueR0;
  }

 /**
 * Get R0 value
 */
 float MQ131Class::getR0() {
  return valueR0;
 }

MQ131Class MQ131(MQ131_DEFAULT_RL);