
#include "main.hpp"

/*
   ESP32 ADC multi readings

   All time ADC pins:
   ADC1_CH0,ADC1_CH03,ADC1_CH04
   ADC1_CH05,ADC1_CH06,ADC1_CH07

   Use only without WiFi:
   ADC2_CH0,ADC2_CH01,ADC2_CH02,ADC2_CH03
   ADC2_CH04,ADC2_CH05,ADC2_CH06
   ADC2_CH07,ADC2_CH08,ADC2_CH09

   Arduino espressif doc: https://goo.gl/NpUo3Z
   Espressif doc: https://goo.gl/hcUy5U
   GPIO: https://goo.gl/k8FGGD
*/

// Command to see the REF_VOLTAGE: espefuse.py --port /dev/ttyUSB0 adc_info
// or dc2_vref_to_gpio(25)
//#define REF_VOLTAGE 1128
//esp_adc_cal_characteristics_t *adc_chars = new esp_adc_cal_characteristics_t;


int id_nodo;
char Devtype;
char VSoft;
char VHard;

int l1i;
int l2i;

/***DHT***/
DHTesp dht;

/**Bluetooth**/
char NAME_BT[14];
BLE BT;

/***epsNOW***/
// REPLACE WITH THE MAC Address of your receiver 7C:9E:BD:ED:5C:78
//uint8_t ESP_NOW_GATEWAY[] = {0x7C, 0x9E, 0xBD, 0xED, 0x5C, 0x78};    //para recibir de todos
uint8_t ESP_NOW_GATEWAY[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};    //para recibir de todos
//uint8_t ESP_NOW_GATEWAY[] = {0x24, 0x6F, 0x28, 0x87, 0xF2, 0x14};   //MAC GW antolin
//uint8_t ESP_NOW_GATEWAY[] = {0xFC, 0xF5, 0xC4, 0x0C, 0xE0, 0xA8};   //MAC GW daniel
uint8_t KEY[16]= {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44};
uint8_t espKEY[16] = {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44};
uint8_t espMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


esp_now_peer_info_t peerInfo;
struct_message myDataSensor;
struct_messagegw myDataGateway;
String success;

//AceButton button(BUTTON_PIN);
// Create 2 ButtonConfigs. The System ButtonConfig is not used.
ButtonConfig config;
AceButton button(&config);

/***Timers***/
unsigned long int LimitTimeToRcv;  //Max. time to recieve from gateway in ms. Comunication with Gateway
unsigned long int timeLastRcv;             //Time of last recieve data from gateway in ms. Comunication with Gateway
unsigned long int SampleTimer;  //Interval of time to send data to gateway in ms.
unsigned long int timeLastSent;            //Time of last send data to gateway in ms.

unsigned long int lastTLedConnect;
boolean ledConnectOn;
unsigned long int lastTLedState;
boolean ledStateOn;
unsigned long int lastTBuzzer;
boolean buzzerOn;
boolean isConnect;
boolean highState; 
//-----------------------------------------------------
// Setup
//-----------------------------------------------------
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  id_nodo = getIdEsp32(); // get id chip
  Devtype = DEVICE_TYPE;            // tipo de decive
  VSoft = VERSION_SOFTWARE;              // versión software
  VHard = VERSION_HARDWARE;              // versión hardware
  LimitTimeToRcv =   TIME_RCV_ESPNOW;  //Max. time to recieve from gateway in ms. Comunication with Gateway
  SampleTimer =      TIME_SEND_ESPNOW;  //Interval of time to send data to gateway in ms.
  highState =        false;    //State of concentration of ozone
  l1i = CONC_LIMIT_SAFE;            //Valores deafult
  l2i = CONC_LIMIT_DESINFECCION;    // Valores default
  isConnect = false;
  ledConnectOn = false;
  ledStateOn = false;
  buzzerOn = true;


    // Initialize the buttons
    // Button uses an external 10k resistor.
  pinMode(BUTTON_PIN, INPUT);
  button.init(BUTTON_PIN, LOW);
  config.setEventHandler(handleEvent);
  config.setFeature(ButtonConfig::kFeatureClick);
  config.setFeature(ButtonConfig::kFeatureDoubleClick);
  config.setFeature(ButtonConfig::kFeatureLongPress);
  config.setFeature(ButtonConfig::kFeatureRepeatPress);
    //buttonState = false;
  //espMAC[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  //espKEY[16] = {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44};
  
  ////// RETRIEVE DATA FROM EEPROM /////////
  //iniEeprom();           

  //////////SHOW MAC ESPNOW ADDRESS /////////
  Serial.println();
  Serial.print("ESP Board Id Number: ");
  Serial.println(id_nodo);
  Serial.print("Device Type: ");
  Serial.println((int)Devtype);
  Serial.print("Hardware Version: ");
  Serial.println((float)VHard/10.0f);
  Serial.print("Software Version: ");
  Serial.println((float)VSoft/10.0f);

  sprintf(NAME_BT,"%i.%i",Devtype, id_nodo);  
  BT.begin(NAME_BT);
  
  //////////SHOW MAC ESPNOW ADDRESS /////////
  Serial.println();
  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
  //while(1){}
  iniLeds();             //Initialize leds y buzzer
  //iniButton();           //Initialize button;
  
  ////////WAIT UNTIL PRESS ON BUTTON//////////
  Serial.println(F("setup(): waiting for button"));
  while (!button.isPressedRaw()) {
  }

  ///////POWER UP SENSOR /////
  pinMode(ONOFF_PIN, OUTPUT);
  digitalWrite(ONOFF_PIN, HIGH);    
  //digitalWrite(POWER_PIN, HIGH); 
  
  /////// BEEP INICIAL //////////////
  ledcWrite(3 , VOLUMEN);
  delay(100);
  ledcWrite(3 , 0); 

  ////////DHT22 INIT//////////////
  dht.setup(DHT_PIN, DHTesp::DHT22);
  Serial.println("DHT iniciado.");

  ///////INICIALIZA PUERTOS ADC ////////
  init_analog();
  
  ////////MQ131 INIT//////////////
  // Init the sensor
  // - Heater control on pin 2 NOT USE, ALWAYS ON
  // - Sensor analog read on OZONE_PIN
  // - Model LOW_CONCENTRATION
  // - Load resistance RL of 1MOhms (1000000 Ohms)
  
  MQ131.begin(OZONE_PIN, LOW_CONCENTRATION, RL);   
  /////// WHEN CALIBRATION NEEDED ////////////
  
  Serial.println("Calibration in progress...");
  MQ131.calibrate();
  Serial.println("Calibration done!");
  Serial.print("R0 = ");
  Serial.print(MQ131.getR0());
  Serial.println(" Ohms");
  //while(1){};
  
 
 // We have our R0 and set it
  //MQ131.setR0(R0_inicial);            /// set R0 del sensor if DEFINE
  Serial.print("Setting R0: ");
  Serial.print(MQ131.getR0());
  Serial.println(" Ohms");
  
  //while(1){}
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); //to prevent the device go to sleep
  connectToGateway(button);
  // Add peer, the gateway with is MAC

  timeLastRcv = millis();           // inicia timer de la ultima vez recibido datos
  timeLastSent = millis();          // inicia timer de la última vez datos mandados
 }

void loop() {
    ledscConnect();
    ledsState();
    //buzzer();
    button.check();
    
    /// Set values to send myDataSensor: int id, float o3, float temp, float hum,float bat, unsigned int readingId
    getData(dht);
    delay(500);    

    /// Check connect to Gateway
    if (millis() - timeLastRcv > LimitTimeToRcv){
      isConnect = false;
      Serial.println("Alert: Connection with Gateway lost.... Trying to connect again");
      connectToGateway(button);
      /*
      if (isConnect == true){ //IF SUCCESS IN CONECTION ADD PEER OF GATEWAY
        addPeer();
      }
      */
    }
  
    
  // Send message via ESP-NOW
   if ((millis() - timeLastSent > SampleTimer) && (isConnect == true)){
    esp_err_t result = esp_now_send(ESP_NOW_GATEWAY, (uint8_t *) &myDataSensor, sizeof(myDataSensor));  
    if (result == ESP_OK) {
         Serial.println("\nPacket sent:");
         Serial.printf("sent: %3u bytes on channel: %i\n", myDataSensor, WiFi.channel());
         Serial.println("Sent with success");
         Serial.print("id: ");
         Serial.print(myDataSensor.id);
         Serial.print(", O3: ");
         Serial.print((float)myDataSensor.o3/10.0f);
         Serial.print(", O3 high: ");
         Serial.print((float)myDataSensor.o3H/10.0f);
         Serial.print(", temp: ");
         Serial.print((float)myDataSensor.temp/10.0f);
         Serial.print(", hum: ");
         Serial.print((float)myDataSensor.hum/10.0f);
         Serial.print(", bat: ");
         Serial.print((float)myDataSensor.bat/100.0f);
         Serial.print(", log: ");
         Serial.println(myDataSensor.log);
    }
    else {
      Serial.println("Error sending the data");
    }
    timeLastSent = millis();
   }
   
}


