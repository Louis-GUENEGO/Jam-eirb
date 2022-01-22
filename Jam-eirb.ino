#include <stdio.h>
#include <SPI.h>
#include "LoRa.h"
#include "DHT.h"
#include "int.h"
#include "Adafruit_APDS9960.h"

// Configuration comunications
#define VISTESSE_USB 115200 // vitesse de l'USB

// Configuration du capteur
#define MIN_TIME_MS 50 // Le temps à partir duquel on ignore le passage (elimination bruit/perturbation)

//#define DEBUG_MODE // Pour voir les données brutes


uint16_t id_node            = 236; // to change
uint16_t id_traffic         = 558; // to change
unsigned int freq = 865500000;//HZ
/*network */ 
#define WITH_APPKEY
unsigned int nCycle = 60;// in second 300
unsigned short id_frame = 0;
#ifdef WITH_APPKEY
uint8_t my_appKey[4]={5, 6, 7, 8};
#endif
unsigned int cpt=1;
char message[100];



unsigned char distance = 0; // le résultat de la conversion A/N
unsigned int time_ms; // le temps en ms du passage
unsigned short int seuil = 6;
unsigned int nbr_auto = 0;



Adafruit_APDS9960 apds; //create the APDS9960 object



void setup() {
    Serial.begin(VISTESSE_USB);
    delay(2000);

    if (!LoRa.begin(freq)) {
      Serial.println("Starting LoRa failed!");
      while (1); 
    }
  
    LoRa.setSpreadingFactor(12);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
    LoRa.setPreambleLength(8);
    LoRa.enableCrc();
    LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);
    
    apds.begin();
    apds.enableProximity(true); //enable proximity mode

    
    time_ms = 0;
    
    tcConfigure();
    tcStartCounter();
    
    //tcDisable();
    nbr_auto = 0;
    //tcDisable();
}



void loop() {
  
  Serial.print("Traffic : ");
  Serial.println(nbr_auto);
  
  Serial.print("Frame Number : ");
  Serial.println(cpt);
  cpt++;
  Serial.print("\n");
    
  /*Send Data via LoRa Module*/
  send_data();

  /* wait for #idlePeriodInMin Minute */
  for (int j=0;j<nCycle;j++) {delay(1000);}

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
        nbr_auto++;
        Serial.print(nbr_auto);
        Serial.print("\n");
        time_ms = 0;
    } else {
        time_ms = 0;
    }
  #endif
}

void send_data(void){
  uint8_t app_key_offset = 0;
  int e;
  #ifdef WITH_APPKEY
      app_key_offset = sizeof(my_appKey);
      memcpy(message,my_appKey,app_key_offset); // on copie my_appKey dans le début du message
  #endif
      uint8_t r_size;
      char final_str[80] = "\\";
      char aux[10] = "";
      char id[1] = "";
      sprintf(final_str, "%s!%i!%hd", final_str,id_node, id_frame++);
     
      sprintf(aux,"%4.1f", (float)nbr_auto);
      sprintf(final_str, "%s#%d/%s", final_str, id_traffic, aux);
      

      r_size=sprintf(message+app_key_offset,final_str);
    
      Serial.print(message);
      Serial.println(r_size);
      
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket(); 
      
      Serial.println("\nDATA sent correctly !");

      nbr_auto = 0;
}
