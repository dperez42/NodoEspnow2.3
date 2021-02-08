#include "main.hpp"

//---------------------------------------------------------
// COMPARE MACs DEL MENSAJE RECIBIDO PARA FILTRAR LO QUE ME INTERESAN
//---------------------------------------------------------
// Check MACs
boolean checkMac(const uint8_t *sender, const uint8_t *receiver){
  int i;
  i=0;
  while (i < 6){
    if (sender[i] != receiver[i]){
      return false;
    }
    i++;
  }
  return true;
}

//-----------------------------------------------------------
// Callback when data is sent
//------------------------------------------------------------
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (DEBUG){
    Serial.print("Last Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  }
  if (status ==0){
    success = "Delivery Success :)";
  }
  else{
    success = "Delivery Fail :(";
  }
}

//-------------------------------------------------------
// Callback when data is received
//--------------------------------------------------------
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
//if (checkMac(mac_addr, ESP_NOW_GATEWAY)){                                             //check if the message is from my gateway
      
      char macStr[18];
      Serial.print("\nPacket received from my gateway: ");
      snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
      Serial.println(macStr);
      memcpy(&myDataGateway, incomingData, sizeof(myDataGateway));                      // save data in incomingData variable
      Serial.print("Bytes received: ");
      Serial.println(len);
      Serial.println("Para device Type: " + myDataGateway.DevType);
      if (l1i != myDataGateway.l1 || l2i != myDataGateway.l2){                          // si data cambia valores grabo en eeprom
              Serial.println("New config recieved");
              l1i = myDataGateway.l1;
              l2i = myDataGateway.l2;
              Serial.print("límite inferior = ");
              Serial.print(l1i);
              Serial.print(", límite desinfección = ");
              Serial.println(l2i);
              Serial.println(" Saved to Eeprom.");
              saveToEeprom(0,String(l1i));
              saveToEeprom(50,String(l2i));
      } else {
        Serial.println(" No changes in Configuration.");
      }
      timeLastRcv = millis();
      isConnect = true;  
}

//--------------------------------------------------------------------
//ADD PEER TO RED  ESPNOW
//--------------------------------------------------------------------
void addPeer(){
      // Register peer
      memcpy(peerInfo.peer_addr, espMAC, 6);
      memcpy(peerInfo.lmk, espKEY, 16);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
      
      // Add peer        
      if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
      }
}

//------------------------------------------------------------------
// connect gateway via ESPNOW
//------------------------------------------------------------------
boolean connectToGateway(AceButton &button){
  
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false); //to prevent the device go to sleep
  Serial.println(WiFi.macAddress());
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return (false);
    //ESP.restart();
  }
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);
  esp_now_set_pmk(espKEY);
  addPeer();
  
  //isConnect = true;
  //return (true);

  int32_t n = WiFi.scanNetworks();
  //recorro todas las redes y pruebo en cada canal Solo las bandas de 1 a 13.
  Serial.print("Number of networks: ");
  Serial.println(n);
  for (uint8_t i=1; i<n; i++) { //n
                Serial.print("Trying with network: ");
                Serial.println(WiFi.SSID(i));
                //WiFi.printDiag(Serial); // Uncomment to verify channel number before
                esp_wifi_set_ps(WIFI_PS_NONE);
                esp_wifi_set_promiscuous(true);
                esp_wifi_set_channel(WiFi.channel(i), WIFI_SECOND_CHAN_NONE);
                esp_wifi_set_promiscuous(false);
                // WiFi.printDiag(Serial); // Uncomment to verify channel change after
                // Init ESP-NOW
                if (esp_now_init() != ESP_OK) {
                  Serial.println("Error initializing ESP-NOW");
                  ESP.restart();
                }
                // Register for a callback function that will be called when data is received
                esp_now_register_recv_cb(OnDataRecv);
                // WAITING FOR RECIEVE from GATEWAY 3 s
                unsigned long initTime = millis();
                Serial.println("wait 3 seconds to recieved some data from gateway");
                while ((millis()-initTime)<3000){
                  ledscConnect();
                  ledsState();
                  //buzzer();
                  button.check();
                }
                if (isConnect == true){
                  Serial.println("IS CONNECTED TO GATEWAY");
                  //Serial.println(WiFi.SSID(i));
                  timeLastRcv = millis();
                  //ssid = WiFi.SSID(i);
                  Serial.print("------Channel: ");
                  Serial.println( WiFi.channel(i));
                  //wifiChannel = WiFi.channel(i);
                  return (true);
                }
  }
  Serial.println("---- No Gateway found -----");
  return(false);
}
