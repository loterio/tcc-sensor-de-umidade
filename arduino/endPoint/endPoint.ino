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
const unsigned long DELAY_ESTABILIZAR = 5000;

const unsigned long DELAY_ENVIAR_NRF = 100;
const unsigned long DELAY_RECEBER_NRF = 100;
const unsigned long TEMPO_TENTANDO_ENVIAR = 10000;
const unsigned long TEMPO_ESPERANDO_DADO_ROOT = 30000;
const unsigned long TEMPO_ESPERANDO_AUTORIZACAO = 30000;
const unsigned long DELAY_SUCESSO = 120000;
const unsigned long DELAY_MINIMO_INSUCESSO = 10000;
const unsigned long DELAY_MAXIMO_INSUCESSO = 30000;

int idSensor;
float umidade;
float longitude;
float latitude;

int idCadastroSensor = 0;

String msgToken;
String msgIdSensor;
String msgUmidade;
String msgLongitude;
String msgLatitude;

unsigned long DELAY_LOOP = 0;

//================{Funções}================
//Dados
void EEPROMWriteInt(int address, int value);
int EEPROMReadInt(int address);
void pegaIdSensor();
void atualizaValores();

//Comunicação
bool enviaDadoNRF(String dado, unsigned long tempoTentandoEnviar);
String recebeDadoNRF(unsigned long tempoEsperandoReceber);
String enviaDadoJsonNRF(String dado, unsigned long tempoTentandoEnviar);
bool recebeDadoJsonNRF(int indiceDado, unsigned long tempoEsperandoDado);
bool validaDadoJsonRecebido(String dadoJson, int indiceDado);
//=========================================

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1, addresses[1]);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  delay(DELAY_ESTABILIZAR);
  pegaIdSensor();
}

void loop() {
  Serial.println("============{Loop - EndPoint}=========");
  DELAY_LOOP = random(DELAY_MINIMO_INSUCESSO, DELAY_MAXIMO_INSUCESSO);
  atualizaValores();
  String dadosJsonSensor[5] = {msgToken, msgIdSensor, msgUmidade, msgLongitude, msgLatitude};
  String respostaRoot = enviaDadoJsonNRF(dadosJsonSensor[0], TEMPO_TENTANDO_ENVIAR);
  if (respostaRoot == SUCESSO) {
    String autorizacaoRoot = recebeDadoNRF(TEMPO_ESPERANDO_AUTORIZACAO);
    if (autorizacaoRoot == AUTORIZADO) {
      String dadoFoiEnviado;
      for (int i = 1; i <= 4; i++) {
        dadoFoiEnviado = enviaDadoJsonNRF(dadosJsonSensor[i], TEMPO_TENTANDO_ENVIAR);
        if (dadoFoiEnviado != SUCESSO) {
          i = 5;
        }
      }
      if (dadoFoiEnviado == SUCESSO) {
        Serial.println("Os Dados da Minha Leitura Foram enviados Com Sucesso!");
        DELAY_LOOP = DELAY_SUCESSO;
      } else {
        Serial.println("Os Dados da Minha Leitura NÃO Foram enviados!");
      }
    }
  } else {
    Serial.println("Os Dados da Minha Leitura NÃO Foram enviados!");
  }
  Serial.println("======================================");
  delay(DELAY_LOOP);
}

void atualizaValores() {
  umidade = 23.75;
  longitude = -49.93047826;
  latitude = -27.93047829;

  msgToken = "{\"t\":\"t[" + TOKEN + "]\"}";
  msgIdSensor = "{\"id\":\"id[" + String(idSensor) + "]\"}";
  msgUmidade = "{\"umi\":\"umi[" + String(umidade, 2) + "]\"}";
  msgLongitude = "{\"lon\":\"lon[" + String(longitude, 8) + "]\"}";
  msgLatitude = "{\"lat\":\"lat[" + String(latitude, 8) + "]\"}";
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

void pegaIdSensor() {
  Serial.println("Vou Pegar Meu ID");
  idSensor = EEPROMReadInt(0);
  while (idSensor == 0) {
    Serial.println("Não tenho Id, Vou Pegar Um");
    DELAY_LOOP = random(DELAY_MINIMO_INSUCESSO, DELAY_MAXIMO_INSUCESSO);
    atualizaValores();
    String dadosJsonSensor[5] = {msgToken, msgIdSensor, msgUmidade, msgLongitude, msgLatitude};
    String respostaRoot = enviaDadoJsonNRF(dadosJsonSensor[0], TEMPO_TENTANDO_ENVIAR);
    if (respostaRoot == SUCESSO) {
      String autorizacaoRoot = recebeDadoNRF(TEMPO_ESPERANDO_AUTORIZACAO);
      if (autorizacaoRoot == AUTORIZADO) {
        String dadoFoiEnviado;
        for (int i = 1; i <= 4; i++) {
          dadoFoiEnviado = enviaDadoJsonNRF(dadosJsonSensor[i], TEMPO_TENTANDO_ENVIAR);
          if (dadoFoiEnviado != SUCESSO) {
            i = 5;
          }
        }
        if (dadoFoiEnviado == SUCESSO) {
          Serial.println("Todos os meu Dados de Cadastro Foram Enviados!!!");
          String idCadastroJson = recebeDadoNRF(TEMPO_ESPERANDO_DADO_ROOT);
          if (idCadastroJson != INSUCESSO) {
            Serial.println("ID Json Recebido: " + idCadastroJson);
            DeserializationError err = deserializeJson(doc, idCadastroJson);
            if (!err) {
              idSensor = doc["idSensor"];     
              Serial.println("ID Sensor: " + String(idSensor));
              EEPROMWriteInt(0, idSensor);
              DELAY_LOOP = DELAY_SUCESSO;
            }
            Serial.println("Recebi Um Id!!!");
          } else {
            Serial.println("Nenhum Id Recebido");
          }
        }
      }
    }
    delay(DELAY_LOOP);
  }
}

bool enviaDadoNRF(String dado, unsigned long tempoTentandoEnviar) {
  bool dadoFoiEnviado = false;
  char dadoChar[32];
  unsigned long momentoInicial = millis();

  dado.toCharArray(dadoChar, 32);

  while ((millis() - momentoInicial) <= tempoTentandoEnviar) {
    delay(DELAY_ENVIAR_NRF);
    if (radio.write(&dadoChar, sizeof(dadoChar))) {
      dadoFoiEnviado = true;
      momentoInicial -= tempoTentandoEnviar;
      //Serial.println("Dado Foi Enviado");
    } else {
      //Serial.println("Erro ao Enviar");
    }
  }
  return dadoFoiEnviado;
}

String recebeDadoNRF(unsigned long tempoEsperandoDado) {
  char dadoRecebido[32] = "insucesso";
  unsigned long momentoInicial = millis();

  radio.startListening();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
    if (radio.available()) {
      radio.read(&dadoRecebido, sizeof(dadoRecebido));
      momentoInicial -= tempoEsperandoDado;
    } else {
      delay(DELAY_RECEBER_NRF);
    }
  }
  radio.stopListening();
  return dadoRecebido;
}

String enviaDadoJsonNRF(String dado, unsigned long tempoTentandoEnviar) {
  String respostaAoEnviar = INSUCESSO;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoTentandoEnviar) {
    if (enviaDadoNRF(dado, tempoTentandoEnviar)) {
      respostaAoEnviar = recebeDadoNRF(tempoTentandoEnviar);
      momentoInicial -= tempoTentandoEnviar;
    }
  }
  return respostaAoEnviar;
}

bool recebeDadoJsonNRF(int indiceDado, unsigned long tempoEsperandoDado) {
  bool dadoJsonFoiRecebido = false;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
    String dadoJsonRecebido = recebeDadoNRF(tempoEsperandoDado);
    Serial.println(dadoJsonRecebido);
    if (dadoJsonRecebido != INSUCESSO) {
      unsigned long tempoEnviarResposta = tempoEsperandoDado - (millis() - momentoInicial);
      if (validaDadoJsonRecebido(dadoJsonRecebido, indiceDado)) {
        if (enviaDadoNRF(SUCESSO, tempoEnviarResposta)) {
          dadoJsonFoiRecebido = true;
          momentoInicial -= tempoEsperandoDado;
        }
      } else {
        enviaDadoNRF(INSUCESSO, tempoEnviarResposta);
      }
    }
  }
  return dadoJsonFoiRecebido;
}

bool validaDadoJsonRecebido(String dadoJson, int indiceDado) {
  return true;
}
