  #include <SoftwareSerial.h>
#include <SPI.h>
#include "RF24.h"

void iniciarNRF();
String retornaLeituraString(int);

const int quantidadeSensores = 2;
bool verificacaoLeitura[quantidadeSensores] = {false, false};
float leitura[quantidadeSensores];


//NRF
RF24 radio(9, 10);
byte addresses[][6] = {"1Node", "2Node", "3Node", "4Node", "5Node"};


//Lora
SoftwareSerial lora(2, 3); //TX, RX

void setup() {
  Serial.begin(9600);
  lora.begin(9600);
  iniciarNRF();
}

void loop() {
  float umidade;
  uint8_t sensor;
  if (radio.available(&sensor)) {                                                                  // Variable for the received timestamp
    while (radio.available()) {                                   // While there is data ready
      radio.read( &umidade, sizeof(umidade) );             // Get the payload
    }
    leitura[sensor - 1] = umidade;
    verificacaoLeitura[sensor - 1] = true;
    if (leituraCompleta()) {
      for (int i = 0; i < quantidadeSensores; i++) {
        verificacaoLeitura[i] = false;
      }
      String stringLeitura;
      stringLeitura.concat("<");
      for (int i = 0; i < quantidadeSensores; i++) {
        stringLeitura.concat(i + 1);
        stringLeitura.concat(":");
        stringLeitura.concat(leitura[i]);
        if (i != (quantidadeSensores - 1)) {
          stringLeitura.concat("|");
        }
      }
      stringLeitura.concat(">");
      Serial.println(stringLeitura);
      lora.println(stringLeitura);
      delay(1000);
    }
  }
}

void iniciarNRF() {
  radio.begin();
  radio.setPALevel(RF24_PA_LOW);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.openReadingPipe(2, addresses[2]);
  radio.openReadingPipe(3, addresses[3]);
  radio.openReadingPipe(4, addresses[4]);
  radio.startListening();
}

bool leituraCompleta() {
  bool result = true;
  for (int i = 0; i < quantidadeSensores; i++) {
    if (verificacaoLeitura[i] == false) {
      result = false;
    }
  }
  return result;
}
