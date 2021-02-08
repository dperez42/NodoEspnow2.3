#include "main.hpp"

//AceButton button(BUTTON_PIN);

//--------------------------------------------------------------------
// INIT LEDS AND BUZZER
//--------------------------------------------------------------------
void iniLeds(){
  // LEDS ledcWrite(channel , volume) ledcSetup(PWM channel(0 to 15), PWM frequency(5000 to 8000), resolution (1 to 16)) ledcAttachPin(pin, channel); 
  ledcSetup(0,5000,12);
  ledcAttachPin(BLUE_PIN,0);
  ledcSetup(1,5000,12);
  ledcAttachPin(GREEN_PIN,1);
  ledcSetup(2,5000,12);
  ledcAttachPin(RED_PIN,2);
  ledcSetup(3,5000,12);
  ledcAttachPin(BUZZER_PIN,3);
  ledcWrite(0 , 0);
  ledcWrite(1 , 0);
  ledcWrite(2 , 0);
}

//--------------------------------------------------------------------
// CONTROL LED CONNECT
//--------------------------------------------------------------------
void ledscConnect(){
  int duration = DELAY_PIN;
  if (isConnect == true){
    ledcWrite(0 , 1500);
    lastTLedConnect = millis();
    return;
  }
  if ((millis() - lastTLedConnect) > duration) {
    if (ledConnectOn == true) {
      ledcWrite(0 , 0); 
      ledConnectOn = false;
    } 
    else {
      ledcWrite(0 , 1500);   
      ledConnectOn = true;
    }
    lastTLedConnect = millis();
  }
  return;
}


//--------------------------------------------------------------------
// CONTROL LED STATE O3
//--------------------------------------------------------------------
void ledsState(){
  int duration = DELAY_PIN;
  if ((millis() - lastTLedState) > duration) {
    if (ledStateOn == true) {
      if (highState == true) {
        ledcWrite(2 , 0);
        ledcWrite(1 , 0);} 
        else {
          ledcWrite(2 , 0);
          ledcWrite(1 , 0);
          }
      ledStateOn = false;
    } 
    else {
      if (highState == true) {
        ledcWrite(2 , 1500);} 
        else {
          ledcWrite(1 , 1500);
          }      
        ledStateOn = true;
    }
    lastTLedState = millis();
  }
  return;
} 


//--------------------------------------------------------------------
// CONTROL BUZZER
//--------------------------------------------------------------------
void buzzer(){
  int duration = DELAY_PIN;
  if (highState == false){
    ledcWrite(3 , 0);
    return;
  }
  if ((millis() - lastTBuzzer) > duration) {
   if (buzzerOn == true) {
      if (highState == true) {
        ledcWrite(3 , 0);} 
      buzzerOn = false;
    } else {
      if (highState == true) {
        ledcWrite(3 , VOLUMEN);}      
        buzzerOn = true;
    }
    lastTBuzzer = millis();
  }
  return;
} 

//---------------------------------------------------------
// Handle buttons
//---------------------------------------------------------
void handleEvent(AceButton* /* button */, uint8_t eventType,
    uint8_t buttonState) {
  // Print out a message for all events.
  Serial.print(F("handleEvent(): eventType: "));
  Serial.print(eventType);
  Serial.print(F("; buttonState: "));
  Serial.println(buttonState);

  // Control the LED only for the Pressed and Released events.
  // Notice that if the MCU is rebooted while the button is pressed down, no
  // event is triggered and the LED remains off.
  switch (eventType) {
    case AceButton::kEventPressed:
      Serial.println("Is pressed");
      //digitalWrite(LED_PIN, LED_ON);
      break;
    case AceButton::kEventReleased:
      Serial.println("Is released");
      //digitalWrite(LED_PIN, LED_OFF);
      break;
    case AceButton::kEventLongPressed:
      Serial.println("Reseting");
      delay(1000);
      digitalWrite(ONOFF_PIN, LOW);
      //buzzer sound
      ledcWrite(3 , VOLUMEN);
      delay(100);
      ledcWrite(3 , 0); 
      delay(100);
      ledcWrite(3 , VOLUMEN);
      delay(100);
      ledcWrite(3 , 0); 
      ESP.restart();
      break;
  }
}

//--------------------------------------------------------------------
// INIT BUTTONS
//--------------------------------------------------------------------
/*
void iniButton(){
  // Button uses an external 10k resistor.
  pinMode(BUTTON_PIN, INPUT);
   // We use the AceButton::init() method here instead of using the constructor
  // to show an alternative. Using init() allows the configuration of the
  // hardware pin and the button to be placed closer to each other.
  button.init(BUTTON_PIN, LOW);
  // Configure the ButtonConfig with the event handler, and enable all higher
  // level events.
  ButtonConfig* buttonConfig = button.getButtonConfig();
  buttonConfig->setEventHandler(handleEvent);
  buttonConfig->setFeature(ButtonConfig::kFeatureClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureDoubleClick);
  buttonConfig->setFeature(ButtonConfig::kFeatureLongPress);
  buttonConfig->setFeature(ButtonConfig::kFeatureRepeatPress);
}
*/