#include <ThingerESP32.h>

#define USERNAME "HenriqueBo1"
#define DEVICE_ID "esp32"
#define DEVICE_CREDENTIAL "SenhaForte"

#define SSID "Paula oi fibra"
#define SSID_PASSWORD "abcdy098"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL);

#include <SoftwareSerial.h>

SoftwareSerial mySerial(23, 22);
String input = "<1:20.00>";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  thing.add_wifi(SSID, SSID_PASSWORD);
  thing["Leitura"] >> [](pson& out){ out = String(input); };
}

void loop() {
  if(mySerial.available() > 1){
    input = mySerial.readString(); 
  }
  thing.handle();
  delay(2000);
}
