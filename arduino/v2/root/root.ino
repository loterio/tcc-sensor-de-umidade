#include <EEPROM.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>

//=================={Json}=================
StaticJsonDocument<256> doc;
//=========================================

//=================={Radio}================
RF24 radio(9, 8); // CE, CSN
byte addresses[][6] = {"1Node", "2Node"};
SoftwareSerial lora(2, 3); //TX, RX
//=========================================

//========={Variaveis e Constantes}========
const String TOKEN = "1.1603836354";
const String SUCESSO = "sucesso";
const String AUTORIZADO = "autorizado";
const String INSUCESSO = "insucesso";
const unsigned long TEMPO_RES_ENDPOINT = 10000;
const unsigned long TEMPO_RES_GATEWAY = 15000;

int idSensor;
int idSensorCadastro;
float umidade;
float longitude;
float latitude;

String leituraCompleta;
//=========================================

//================{Funções}================
//Dados
void esvaziaVariaveis();
void atualizaLeituraCompleta();
void imprimeValores();

//Comunicação
bool enviaDado(String dado, int tentativas);
String recebeDado(unsigned long tempoEspera);
String enviaDadoJson(String dadoJson, unsigned long tempoTentando);
bool recebeDadosJson(unsigned long tempoEsperandoDadoCorreto);
int pegaId(unsigned long tempoEsperandoId);
void enviaDadoLora(String dado);
String recebeDadoLora(unsigned long tempoEspera);
//=========================================

void setup() {
  Serial.begin(9600);
  lora.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.setPALevel(RF24_PA_MIN);
}

void loop() {
  unsigned long momentoInicial = millis();
  esvaziaVariaveis();
  radio.startListening();
  String dadoEndPoint = recebeDado(TEMPO_RES_ENDPOINT);
  if (dadoEndPoint != INSUCESSO) {
    radio.stopListening();
    String resposta = INSUCESSO;
    DeserializationError err = deserializeJson(doc, dadoEndPoint);
    if (!err) {
      char* tokenRecebido = doc["t"];
      String tokenRecebidoStr = String(tokenRecebido);
      if (tokenRecebido > "") {
        if (tokenRecebidoStr == TOKEN) {
          resposta = AUTORIZADO;
        }
      }
    }

    bool dadoFoiEnviado = enviaDado(resposta, 10);
    if (resposta == AUTORIZADO and dadoFoiEnviado) {
      bool dadosJsonRecebidos = recebeDadosJson(TEMPO_RES_ENDPOINT);
      if (dadosJsonRecebidos and idSensor == 0) {
        Serial.println("Vou te mandar um id");
        int novoIdSensor = pegaId(TEMPO_RES_GATEWAY);
        if (novoIdSensor > 0) {
          String msgNovoIdSensor = "{\"id\":" + String(novoIdSensor) + "}";
          enviaDadoJson(msgNovoIdSensor, TEMPO_RES_ENDPOINT);
        } else {
          enviaDado(INSUCESSO, 5);
        }
        imprimeValores();

      } else if (dadosJsonRecebidos and idSensor != 0) {
        Serial.println("Recebi Uma Leitura");
        atualizaLeituraCompleta();
        enviaDadoLora(leituraCompleta);
        unsigned long tempoInicial = millis();
        while ((millis() - tempoInicial) <= TEMPO_RES_GATEWAY) {
          delay(100);
          String respostaGateway = recebeDadoLora(TEMPO_RES_GATEWAY);
          if (respostaGateway == SUCESSO) {
            Serial.println("Enviado Com Sucesso");
            tempoInicial -= TEMPO_RES_GATEWAY;
          } else {
            Serial.print("Resposta Gateway: ");
            Serial.println(respostaGateway);
          }
        }
        imprimeValores();
      }
    }
  }
  unsigned long demoraDeLoop = millis() - momentoInicial;
  Serial.println("Demora do Loop: " + String(demoraDeLoop));
}

int pegaId(unsigned long tempoEsperandoId) {
  atualizaLeituraCompleta();
  enviaDadoLora(leituraCompleta);
  unsigned long tempoInicial = millis();
  while ((millis() - tempoInicial) <= tempoEsperandoId){
    String idJson = recebeDadoLora(tempoEsperandoId);
    Serial.println(idJson);
    DeserializationError err = deserializeJson(doc, idJson);
    if (!err) {
      idSensorCadastro = doc["id"];
      if (idSensorCadastro > 0) {
        enviaDadoLora("[sucesso]");
        tempoInicial-=tempoEsperandoId;
      }
    }  
  }
  return idSensorCadastro;
}

void atualizaLeituraCompleta() {
  leituraCompleta = "[{\"t\":\"" + TOKEN + "\",\"id\":" + String(idSensor) + ",\"umi\":" + String(umidade, 2) + ",\"lon\":" + String(longitude, 14) + ",\"lat\":" + String(latitude, 14) + "}]";
}

void imprimeValores() {
  Serial.print("Id Sensor: ");
  Serial.println(idSensor);
  Serial.print("Umidade: ");
  Serial.println(umidade);
  Serial.print("Longitude: ");
  Serial.println(longitude, 7);
  Serial.print("Latitude: ");
  Serial.println(latitude, 7);
  Serial.print("Leitura Completa: ");
  Serial.println(leituraCompleta);
  Serial.println("============================================");
}

bool recebeDadosJson(unsigned long tempoEsperandoDadoCorreto) {
  bool dadosValidos;
  for (int i = 1; i <= 4; i++) {
    Serial.println("Recebendo Dado: " + String(i));
    unsigned long tempoEsperandoDado = tempoEsperandoDadoCorreto / 10;
    unsigned long tempoInicial = millis();
    String dadoEndPoint;
    while ((millis() - tempoInicial) <= tempoEsperandoDadoCorreto) {
      dadosValidos = false;
      dadoEndPoint = recebeDado(tempoEsperandoDado);
      if (dadoEndPoint != INSUCESSO) {
        DeserializationError err = deserializeJson(doc, dadoEndPoint);
        if (!err) {
          switch (i) {
            case 1:
              idSensor = doc["id"];
              if (idSensor >= 0) {
                dadosValidos = true;
              }
              break;
            case 2:
              umidade = doc["umi"];
              if (umidade >= 0) {
                dadosValidos = true;
              }
              break;
            case 3:
              longitude = doc["lon"];
              if (longitude != 0) {
                dadosValidos = true;
              }
              break;
            case 4:
              latitude = doc["lat"];
              if (latitude != 0) {
                dadosValidos = true;
              }
              break;
          }
        }
      }
      if (dadosValidos == true) {
        enviaDado(SUCESSO, 10);
        tempoInicial -= tempoEsperandoDadoCorreto;
      }
    }
    if (dadosValidos != true) {
      i = 5;
    }
  }
  return dadosValidos;
}

String enviaDadoJson(String dadoJson, unsigned long tempoTentando) {
  String resposta = INSUCESSO;
  unsigned long tempoInicial = millis();
  while ((millis() - tempoInicial) <= tempoTentando) {
    if (enviaDado(dadoJson, 1)) {
      resposta = recebeDado(tempoTentando);
      if (resposta == SUCESSO) {
        tempoInicial-=tempoTentando;
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

void esvaziaVariaveis() {
  idSensor = -1;
  idSensorCadastro = 0;
  umidade = -1;
  longitude = 0;
  latitude = 0;
}

void enviaDadoLora (String dado) {
  lora.print(dado);
}

String recebeDadoLora(unsigned long tempoEspera) {
  String dadoRecebido = INSUCESSO;
  unsigned long tempoInicial = millis();
  while ((millis() - tempoInicial) <= tempoEspera) {
    if (lora.available() > 1) {
      dadoRecebido = lora.readString();
      String stringAux = "";
      int inicioMensagem = dadoRecebido.indexOf("[");
      int fimMensagem = dadoRecebido.indexOf("]");
      if (inicioMensagem != -1 and fimMensagem != -1) {
        for (int i = inicioMensagem; i <= fimMensagem ; i++) {
          stringAux += dadoRecebido[i];
        }
        dadoRecebido = stringAux;
        inicioMensagem = dadoRecebido.indexOf("[");
        dadoRecebido.remove(inicioMensagem, 1);
        fimMensagem = dadoRecebido.indexOf("]");
        dadoRecebido.remove(fimMensagem, 1);
      }
      tempoInicial -= tempoEspera;
      delay(20);
    }
  }
  return dadoRecebido;
}
