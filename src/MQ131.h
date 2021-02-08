#ifndef _MQ131_H_
#define _MQ131_H_

#include <Arduino.h>
//#include "ADCcorrection.h"
#include <driver/adc.h> //libreria para configurar ADC
#include <esp_adc_cal.h>

// Default values
#define MQ131_DEFAULT_RL                            1000000           // Default load resistance of 1MOhms
#define MQ131_DEFAULT_STABLE_CYCLE                  15                // Number of cycles with low deviation to consider
                                                                      // the calibration as stable and reliable
#define MQ131_DEFAULT_TEMPERATURE_CELSIUS           20                // Default temperature to correct environmental drift
#define MQ131_DEFAULT_HUMIDITY_PERCENT              65                // Default humidity to correct environmental drift
#define MQ131_DEFAULT_LO_CONCENTRATION_R0           1917.22           // Default R0 for low concentration MQ131
#define MQ131_DEFAULT_LO_CONCENTRATION_TIME2READ    80                // Default time to read before stable signal for low concentration MQ131
#define MQ131_DEFAULT_HI_CONCENTRATION_R0           235.00            // Default R0 for high concentration MQ131
#define MQ131_DEFAULT_HI_CONCENTRATION_TIME2READ    80                // Default time to read before stable signal for high concentration MQ131

enum MQ131Model {LOW_CONCENTRATION, HIGH_CONCENTRATION};
enum MQ131Unit {PPM, PPB, MG_M3, UG_M3};

class MQ131Class {
  public:
    // Constructor
    MQ131Class(uint32_t _RL);
    virtual ~MQ131Class();
  
    // Initialize the driver
    void begin(uint8_t _pinSensor, MQ131Model _model, uint32_t _RL);

    // Manage a full cycle with delay() without giving the hand back to
    // the main loop (delay() function included)
    void sample();                

    // Read the concentration of gas
    // The environment should be set for accurate results
    float getO3(MQ131Unit unit);

    // Define environment
    // Define the temperature (in Celsius) and humidity (in %) to adjust the
    // output values based on typical characteristics of the MQ131
    void setEnv(int8_t tempCels, uint8_t humPc);

    // Setup calibration: R0
    // Define the R0 for the calibration
    // Get function also available to know the value after calibrate()
    // (the time to read is calculated automatically after calibration)
    void setR0(float _valueR0);
    float getR0();

    // Launch full calibration cycle
    // Ideally, 20°C 65% humidity in clean fresh air (can take some minutes)
    // For further use of calibration values, please use getTimeToRead() and getR0()
    void calibrate();

  private:
    
    // Internal reading function of Rs
    float readRs();

    // Get environmental correction to apply on ration Rs/R0
    float getEnvCorrectRatio();

        // Convert gas unit of gas concentration
        float convert(float input, MQ131Unit unitIn, MQ131Unit unitOut);

        // Internal variables
    // Model of MQ131
    MQ131Model model;

    // Details about the circuit: pins and load resistance value
    uint8_t pinSensor = -1;
    uint32_t valueRL = -1;

    // Calibration of R0
    float valueR0 = -1;

    // Last value for sensor resistance
    float lastValueRs = -1;

    // Parameters for environment
    int8_t temperatureCelsuis = MQ131_DEFAULT_TEMPERATURE_CELSIUS;
    uint8_t humidityPercent = MQ131_DEFAULT_HUMIDITY_PERCENT;
};

extern MQ131Class MQ131;

#endif // _MQ131_H_