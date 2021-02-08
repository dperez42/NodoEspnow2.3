#include "main.hpp"

//---------------------------------------------------
// Save data in EEPROM
//---------------------------------------------------
void saveToEeprom(int addr, String a) {
  int tamano = a.length(); 
  char inchar[50]; 
  a.toCharArray(inchar, tamano+1);
  for (int i = 0; i < tamano; i++) {
    EEPROM.write(addr+i, inchar[i]);
  }
  for (int i = tamano; i < 50; i++) {
    EEPROM.write(addr+i, 255);
  }
  EEPROM.commit();
}

//---------------------------------------------------
// Load data from EEPROM.  (RECUPERA DATOS DEL EEPROM)
//---------------------------------------------------
String loadFromEeprom(int addr) {
   byte lectura;
   String strlectura;
   for (int i = addr; i < addr+50; i++) {
      lectura = EEPROM.read(i);
      if (lectura != 255) {
        strlectura += (char)lectura;
      }
   }
   return strlectura;
}

//--------------------------------------------------------------------
// INIT EEPROM VALUES (l1 Y l2)
//--------------------------------------------------------------------
void iniEeprom(){
  EEPROM.begin(512); //0:limitDownO3c[50], 50:limitUpO3c[50];
  Serial.println("Retrieve from EEPROM");
  loadFromEeprom(0).toCharArray(l1, 50);
  loadFromEeprom(50).toCharArray(l2, 50);
  l1i = atoi(l1);
  l2i = atoi(l2);
  Serial.print("Concentración O3 en ppb no saludable:");
  Serial.println (l1);
  Serial.print("Concentratción O3 en ppb para desinfección:");
  Serial.println (l2);
}