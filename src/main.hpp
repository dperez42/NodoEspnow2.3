#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Wire.h>
#include <EEPROM.h>     //Biblioteca para grabar en EEPROM
#include <AceButton.h>    //https://github.com/bxparks/AceButton
using namespace ace_button;
#include "DHTesp.h"   //biblioteca lectura DHT22
#include <driver/adc.h> //libreria para configurar ADC
#include "MQ131.h"
#include "BLE.h"
#include <esp_adc_cal.h> 

#define DEVICE_TYPE         1;                      // tipo de device (1-255)
#define VERSION_SOFTWARE    11;                     // versión software de 00.0 a 25.5 (0-255)
#define VERSION_HARDWARE    125;                     // version hardware de 00.0 a 25.5  (0-255)
#define TIME_RCV_ESPNOW   2000                      //Tiempo limite para recibir del servidor
#define TIME_SEND_ESPNOW  15000                     //Tiempo para mandar al servidor
#define CONC_LIMIT_SAFE   50                        //Valor por defecto concentración de O3 segura
#define CONC_LIMIT_DESINFECCION 400                 //Valor por defecto concentración de O3 para desinfectar

#define DHT_PIN           4                        //Pin for DHT11
#define OZONE_PIN         33                       //Pin sensor 
#define OZONE_HIGH_PIN    32                       //Pin sensor
#define BLUE_PIN          22                       //Pin led Blue
#define RED_PIN           17                       //Pin led Red
#define GREEN_PIN         18                       //Pin led Green
#define BUZZER_PIN        19                       //Pin Buzzer
#define VOLUMEN           1024                     //Volumen buzzer
#define BUTTON_PIN        34                       //Pin Button
#define ONOFF_PIN         23                       //Pin Led ON/OFF
#define POWER_PIN         21                       //Pin to Power sensors
#define DELAY_PIN         500                      //Frecuencia blinking leds and buzzer
#define BAT_PIN           35                       //Pin  GPIO35
#define R0_inicial        105000                   //Value of R0 5,15 y 3,32 ->105000, muy sensible a la alimentación de heater
#define RL                470000                  //Value of RL


#define DEBUG true
#define LOG   true

extern int id_nodo;
extern char Devtype;
extern char VSoft;
extern char VHard;



extern char l1[25]; //concentracion de ozono peligrosa en ppb (50)
extern char l2[25]; //concentracion de ozono en desinfección en ppb (400)
extern int l1i;
extern int l2i;

/***WIFI***/
extern boolean isConnect;
extern int wifiChannel;

extern unsigned long int timeLastRcv;             //Time of last recieve data from gateway in ms. Comunication with Gateway
extern boolean highState;    //State of concentration of ozone

extern unsigned long int lastTLedConnect;
extern boolean ledConnectOn;
extern unsigned long int lastTLedState;
extern boolean ledStateOn;
extern unsigned long int lastTBuzzer;
extern boolean buzzerOn;
extern boolean buttonState;

// REPLACE WITH THE MAC Address of your receiver 7C:9E:BD:ED:5C:78
//uint8_t ESP_NOW_GATEWAY[] = {0x7C, 0x9E, 0xBD, 0xED, 0x5C, 0x78};    //para recibir de todos
extern uint8_t espMAC[6]; // = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};    //para recibir de todos
//uint8_t ESP_NOW_GATEWAY[] = {0x24, 0x6F, 0x28, 0x87, 0xF2, 0x14};   //MAC GW antolin
//uint8_t ESP_NOW_GATEWAY[] = {0xFC, 0xF5, 0xC4, 0x0C, 0xE0, 0xA8};   //MAC GW daniel
extern uint8_t espKEY[16]; //= {0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44, 0x33, 0x44};
extern esp_now_peer_info_t peerInfo;
// Structure to send data
// Must match the sender structure
typedef struct struct_message {
  unsigned long int id;                         //id
  unsigned char     DevType;
  unsigned char     VSoft;
  unsigned char     VHard;
  unsigned int      o3;                       //O3 Low
  unsigned int      o3H;                      //O3 High
  unsigned int      temp;                     //Temperature
  unsigned int      hum;                      //Humidity
  unsigned char     bat;                      //Battery       
  int               log;
} struct_message;

extern struct_message myDataSensor;

// Structure to recieve data
// Must match the sender structure 
typedef struct struct_messagegw {
   unsigned char     DevType;
  unsigned int l1;                           //limite salud
  unsigned int l2;                          // límite de desinfección
} struct_messagedw;

extern struct_messagegw myDataGateway;

// Variable to store if sending data was successful
extern String success;





void iniLeds();
void ledscConnect();
void ledsState();
void buzzer();
void handleEvent(AceButton* /* button */, uint8_t eventType,uint8_t buttonState);
//void iniButton();

void init_analog();

uint32_t getIdEsp32();

float checkBattery(int battery_pin);

extern boolean checkMac(const uint8_t *sender, const uint8_t *receiver);
extern void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
extern void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len);
void addPeer();
boolean connectToGateway(AceButton &button);

void getData(DHTesp dht);

void iniEeprom();
String loadFromEeprom(int addr);
void saveToEeprom(int addr, String a);

#endif
