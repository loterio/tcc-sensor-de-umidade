const int arduinoMaster = 0;
const int esteSensor = 1;

#include <SPI.h>
#include "RF24.h"

void iniciaNRF(int, int); 
void enviaDado(float dado);
float umidadeDoSolo();

RF24 radio(9, 10);
byte addresses[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node"};

//============================================

void setup() {
  Serial.begin(9600);
  iniciaNRF(esteSensor, arduinoMaster); 
}

//============================================

void loop() {
 enviaDado(umidadeDoSolo());
 delay(10000);
}

//============================================

void iniciaNRF(int esteSensor, int arduinoMaster) {
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[esteSensor]);
  radio.openReadingPipe(1, addresses[arduinoMaster]);
  radio.startListening();
}

void enviaDado(float dado){
  radio.stopListening();   
  // First, stop listening so we can talk.
  Serial.println(F("Enviando Agora!!!"));
  Serial.println(dado);
  if (!radio.write(&dado, sizeof(dado))) {
    Serial.println(F("Não conectou"));
    for(int i = 1; i <= 10; i++){
      Serial.print("Não foi, Tentativa: ");
      Serial.println(i);
      if (radio.write(&dado, sizeof(dado))){
        i = 11;
      }
      int espera = random(200, 2000);
      delay(espera);
    }
  }
  radio.startListening();                                    // Now, continue listening
}

float umidadeDoSolo(){
  return random(20, 30);
}
