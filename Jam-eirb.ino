// Configuration CAN
#define PIN_LIDAR 15 // 15 => pin A0
#define RESOLUTION 12 // ADC 12 bits => 4096 values

// Configuration comunications
#define VISTESSE_USB 115200 // vitesse de l'USB

// Configuration du capteur
#define THRESHOLD 1000 // en quantum, le niveau à paritr du quel on considère les signaux
#define MIN_TIME_MS 30 // Le temps à partir duquel on ignore le passage (elimination bruit/perturbation)

//#define DEBUG_MODE





unsigned short int resultCAN = 0; // le résultat de la conversion A/N
unsigned int time_ms; // le temps en ms du passage



void setup() {
    analogReadResolution(RESOLUTION); // configuration du CAN

    time_ms = 0;
    
    tcConfigure();
    tcStartCounter();
}



void loop() {
  
  //void

}




void TC5_Handler (void) { // TC5 interrupt routine
  
  resultCAN = analogRead(PIN_LIDAR); // Conversion A/N
  #ifdef DEBUG_MODE
    Serial.print(resultCAN);
    Serial.print(" Quantum\n");
  #else
    if ( resultCAN > THRESHOLD ){
      if (time_ms == 0) {
          time_ms = 1;
      } else {
        time_ms++;
      }
    } else if ( time_ms > MIN_TIME_MS ) {
        Serial.print(time_ms);
        Serial.print(" ms\n");
        time_ms = 0;
    }
  #endif
  
  TC5->COUNT16.INTFLAG.bit.MC0 = 1; // clear interrup flag
}







void tcConfigure(void) {
    
    GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5)) ; // select the generic clock generator used as source to the generic clock multiplexer
    while (GCLK->STATUS.bit.SYNCBUSY);
    
    tcReset(); //reset TC5
    
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16; // Set Timer counter 5 Mode to 16 bits
    
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC5 waveform generation mode to 'match frequency'
    
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1 | TC_CTRLA_ENABLE; // set prescaler to 1
    
    TC5->COUNT16.CC[0].reg = (uint16_t) 48000; //set the compare-capture register. 48000 => 1ms
    while (tcIsSyncing());
    
    // Configure interrupt request
    NVIC_DisableIRQ(TC5_IRQn);
    NVIC_ClearPendingIRQ(TC5_IRQn);
    NVIC_SetPriority(TC5_IRQn, 0);
    NVIC_EnableIRQ(TC5_IRQn);
    
    TC5->COUNT16.INTENSET.bit.MC0 = 1;// Enable the TC5 interrupt request
    while (tcIsSyncing()); //wait until TC5 is done syncing 
} 



bool tcIsSyncing() { //Function that is used to check if TC5 is done syncing
    return TC5->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY; //returns true when it is done syncing
}

void tcStartCounter() { //This function enables TC5 and waits for it to be ready
    TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; //set the CTRLA register
    while (tcIsSyncing()); //wait until snyc'd
}

void tcReset() { //Reset TC5
    TC5->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;
    while (tcIsSyncing());
    while (TC5->COUNT16.CTRLA.bit.SWRST);
}

void tcDisable() { //disable TC5
    TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (tcIsSyncing());
}
