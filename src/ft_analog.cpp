#include "main.hpp"

void init_analog(){
 // -- Fixed for the all adc1 ---
  // 4095 for 12-bits -> VDD_A / 4095 = 805uV  too jittery
  // 2048 for 11-bits -> 3.9 / 2048 = 1.9mV looks fine
  /*
     Set the resolution of analogRead return values. Default is 12 bits (range from 0 to 4096).
     If between 9 and 12, it will equal the set hardware resolution, else value will be shifted.
     Range is 1 - 16
  */
  analogReadResolution(12); // https://goo.gl/qwUx2d
 /*
  * Sets the sample bits and read resolution
  * Default is 12bit (0 - 4095)
  * Range is 9 - 12
  * */
  analogSetWidth(12);
  
  /*
  * Set number of cycles per sample
  * Default is 8 and seems to do well
  * Range is 1 - 255
  * */
  analogSetCycles(8);

  /*
  * Set number of samples in the range.
  * Default is 1
  * Range is 1 - 255
  * This setting splits the range into
  * "samples" pieces, which could look
  * like the sensitivity has been multiplied
  * that many times
  * */
  analogSetSamples(1);

    /*
  * Set the divider for the ADC clock.
  * Default is 1
  * Range is 1 - 255
  * */
  analogSetClockDiv(1);
  /*
  * Set the attenuation for all channels
  * Default is 11db
  * */
  //analogSetAttenuation(ADC_11db); //ADC_0db, ADC_2_5db, ADC_6db, ADC_11db

  /*
  * Set the attenuation for particular pin
  * Default is 11db
  * */
  analogSetPinAttenuation(OZONE_PIN, ADC_11db); //ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
   analogSetPinAttenuation(OZONE_HIGH_PIN, ADC_11db); //ADC_0db, ADC_2_5db, ADC_6db, ADC_11db
  analogSetPinAttenuation(BAT_PIN, ADC_11db);
  /*
  * Attach pin to ADC (will also clear any other analog mode that could be on)
  * */
  adcAttachPin(OZONE_PIN);
  adcAttachPin(OZONE_HIGH_PIN);
  adcAttachPin(BAT_PIN);
   
  // Calibration function
  //esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_11, REF_VOLTAGE, adc_chars);
  /*  
 adc1_config_width(ADC_WIDTH_BIT_12); //Default od 12 is nor very linear use 10(0-1023) o 11 
 adc1_config_channel_atten(ADC1_CHANNEL_7, ADC_ATTEN_DB_11); //GPIO 35 BAT_PIN, Default 11db is very noisy use 2.5 or 6
  //adc1_config_channel_atten(ADC1_CHANNEL_4, ADC_ATTEN_DB_6); //GPIO 32, Default 11db is very noisy use 2.5 or 6
 adc1_config_channel_atten(ADC1_CHANNEL_5, ADC_ATTEN_DB_11); //GPIO 33 OZONE_PIN , Default 11db is very noisy use 2.5 or 6
  */
}