#include "int.h"
#include "Adafruit_APDS9960.h"

// Configuration CAN
#define PIN_LIDAR 15 // 15 => pin A0
#define RESOLUTION 12 // ADC 12 bits => 4096 values

// Configuration comunications
#define VISTESSE_USB 115200 // vitesse de l'USB

// Configuration du capteur
#define THRESHOLD 800 // en quantum, le niveau à paritr du quel on considère les signaux
#define MIN_TIME_MS 30 // Le temps à partir duquel on ignore le passage (elimination bruit/perturbation)

//#define DEBUG_MODE // Pour voir les données brutes





unsigned char distance = 0; // le résultat de la conversion A/N
unsigned int time_ms; // le temps en ms du passage
unsigned short int seuil = 8;
unsigned int nbr_auto = 0;
unsigned char voie = 3;





Adafruit_APDS9960 apds; //create the APDS9960 object



void setup() {
    Serial.begin(VISTESSE_USB);
    analogReadResolution(RESOLUTION); // configuration du CAN
    
    apds.begin();
    apds.enableProximity(true); //enable proximity mode

    
    time_ms = 0;
    
    tcConfigure();
    tcStartCounter();
}



void loop() {
  
  //void

}




void routine (void) { // interrupt routine (1kHz)
  
  distance = apds.readProximity();
  
  #ifdef DEBUG_MODE
    Serial.print(distance);
    Serial.print("\n");
  #else

  
    if ( distance > seuil ){
        
        time_ms++;
       
    } else if ( time_ms > MIN_TIME_MS ) {
        Serial.print(time_ms);
        Serial.print(" ms\n");
        time_ms = 0;
    } else {
        time_ms = 0;
    }
  #endif
}
