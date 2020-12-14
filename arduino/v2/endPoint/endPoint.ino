#include <EEPROM.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//=================={Json}=================
StaticJsonDocument<256> doc;
//=========================================

//=================={Radio}================
RF24 radio(9, 8); // CE, CSN
byte addresses[][6] = {"1Node", "2Node"};
//=========================================

//========={Variaveis e Constantes}========
const String TOKEN = "1.1603836354";
const String SUCESSO = "sucesso";
const String AUTORIZADO = "autorizado";
const String INSUCESSO = "insucesso";
const unsigned long DELAYSUCESSO = 30000;
const unsigned long DELAYINSUCESSO = 7000;
const unsigned long TEMPO_RES_ROOT = 10000;
const unsigned long TEMPO_RES_GATEWAY = 20000;

int idSensor;
float umidade;
float longitude;
float latitude;

String msgToken;
String msgIdSensor;
String msgUmidade;
String msgLongitude;
String msgLatitude;
//=========================================

//================{Funções}================
//Dados
int retornaIdSensor();
float pegaUmidade(int port);
float pegaLongitude();
float pegaLatitude();
void atualizaValores();
void imprimeValores();
void EEPROMWriteInt(int address, int value);
int  EEPROMReadInt(int address);

//Comunicação
bool enviaDado(String dado, int tentativas);
String recebeDado(unsigned long tempoEspera);
String enviaDadoJson(String dadoJson, unsigned long tempoTentando);
bool enviaLeitura();
//=========================================



void setup() {
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  delay(5000);
  idSensor = retornaIdSensor();
  Serial.println(idSensor);
  Serial.println(EEPROMReadInt(0));
}

void loop() {
  unsigned long momentoInicial = millis();
  if(enviaLeitura()){
    Serial.println("Foi Enviado");
    delay(DELAYSUCESSO);
  } else {
    Serial.println("Deu Merda");
    delay(DELAYINSUCESSO);
  }
  unsigned long demoraDeLoop = millis() - momentoInicial;
  Serial.println("Demora do Loop: " + String(demoraDeLoop));
}

bool enviaLeitura() {
  bool leituraEnviada = false;
  atualizaValores();
  String mensagens[5] = {msgToken, msgIdSensor, msgUmidade, msgLongitude, msgLatitude};
  String respostaRoot = INSUCESSO;
  if (enviaDado(mensagens[0], 10)) {
    respostaRoot = recebeDado(TEMPO_RES_ROOT);
    if (respostaRoot == AUTORIZADO) {
      Serial.println("Estou Autorizado");
      for (int i = 1; i <= 4; i++) {
        Serial.println("Enviando Dado: " + String(i));
        respostaRoot = enviaDadoJson(mensagens[i], TEMPO_RES_ROOT);
        if (respostaRoot != SUCESSO) {
          i = 5;
        }
      }
    } else {
      Serial.println("Não Estou Autorizado");
    }
  }
  if (respostaRoot == SUCESSO) {
    leituraEnviada = true;
  }
  return leituraEnviada;
}

int retornaIdSensor() {
  int idSensor = EEPROMReadInt(0);
  while (idSensor == 0) {
    atualizaValores();
    String mensagens[5] = {msgToken, msgIdSensor, msgUmidade, msgLongitude, msgLatitude};
    String respostaRoot = INSUCESSO;
    delay(500);
    if (enviaDado(mensagens[0], 10)) {
      respostaRoot = recebeDado(TEMPO_RES_ROOT);
      if (respostaRoot == AUTORIZADO) {
        for (int i = 1; i <= 4; i++) {
          Serial.println("Enviando dado: "+ String(i));
          respostaRoot = enviaDadoJson(mensagens[i], TEMPO_RES_ROOT);
          if (respostaRoot != SUCESSO) {
            i = 5;
          }
        }

        if (respostaRoot == SUCESSO) {
          respostaRoot = recebeDado(TEMPO_RES_GATEWAY);
          Serial.println(respostaRoot);
          DeserializationError err = deserializeJson(doc, respostaRoot);
          if (!err) {
            if (doc["id"] > 0) {
              enviaDado(SUCESSO, 15);
              idSensor = doc["id"];
              EEPROMWriteInt(0, idSensor);
            }
          } else {
            delay(DELAYINSUCESSO);
          }
        }
      }
    }
  }
  return idSensor;
}

String enviaDadoJson(String dadoJson, unsigned long tempoTentando) {
  String resposta = INSUCESSO;
  unsigned long tempoInicial = millis();
  while ((millis() - tempoInicial) <= tempoTentando) {
    if (enviaDado(dadoJson, 1)) {
      resposta = recebeDado(tempoTentando);
      Serial.println("Dado Recebido" + String(resposta));
      if (resposta == SUCESSO) {
        tempoInicial -= tempoTentando;
      }
    }
  }
  return resposta;
}

bool enviaDado(String dado, int tentativas) {
  char msg[32];
  dado.toCharArray(msg, 32);
  bool enviado = radio.write(&msg, sizeof(msg));
  for (int i = 0; i < tentativas; i++) {
    if (enviado) {
      i = tentativas;
    } else {
      int intervalo = random(400, 500);
      delay(intervalo);
      Serial.print("Erro: ");
      Serial.println(i);
      enviado = radio.write(&msg, sizeof(msg));
    }
  }
  return enviado;
}

String recebeDado(unsigned long tempoEspera) {
  char resposta[32] = "insucesso";
  radio.startListening();
  unsigned long tempoInicial = millis();
  while ((millis() - tempoInicial) <= tempoEspera) {
    if (radio.available()) {
      radio.read(&resposta, sizeof(resposta));
      tempoInicial -= tempoEspera;
    }
  }
  radio.stopListening();
  return resposta;
}

void EEPROMWriteInt(int address, int value) {
  byte hiByte = highByte(value);
  byte loByte = lowByte(value);
  EEPROM.write(address, hiByte);
  EEPROM.write(address + 1, loByte);
}

int EEPROMReadInt(int address) {
  byte hiByte = EEPROM.read(address);
  byte loByte = EEPROM.read(address + 1);
  return word(hiByte, loByte);
}

void imprimeValores() {
  String mensagens[5] = {msgToken, msgIdSensor, msgUmidade, msgLongitude, msgLatitude};
  for (int i = 0; i < 5; i++) {
    Serial.println(mensagens[i]);
  }
}

void atualizaValores() {
  umidade = pegaUmidade(1);
  longitude = pegaLongitude();
  latitude = pegaLatitude();

  msgToken = "{\"t\":\"" + TOKEN + "\"}";
  msgIdSensor = "{\"id\":" + String(idSensor) + "}";
  msgUmidade = "{\"umi\":" + String(umidade, 2) + "}";
  msgLongitude = "{\"lon\":" + String(longitude, 14) + "}";
  msgLatitude = "{\"lat\":" + String(latitude, 14) + "}";
}

float pegaUmidade(int port) {
  return 23.84;
}

float pegaLongitude() {
  return -49.58769123849318;
}

float pegaLatitude() {
  return -27.93841428489864;
}
