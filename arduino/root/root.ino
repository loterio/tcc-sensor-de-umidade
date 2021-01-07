#include <SoftwareSerial.h>
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
SoftwareSerial lora(2, 3); //TX, RX
//=========================================

//========={Variaveis e Constantes}========
const String TOKEN = "1.1603836354";
const String SUCESSO = "sucesso";
const String AUTORIZADO = "autorizado";
const String INSUCESSO = "insucesso";
const unsigned long DELAY_ESTABILIZAR = 5000;

const unsigned long DELAY_ENVIAR_NRF = 100;
const unsigned long DELAY_RECEBER_NRF = 100;
const unsigned long TEMPO_ESPERANDO_DADO_SENSOR = 20000;
const unsigned long TEMPO_ENVIAR_DADO_SENSOR = 5000;
const unsigned long TEMPO_ENVIANDO_AUTORIZACAO = 20000;

const unsigned long DELAY_ENVIAR_LORA = 200;
const unsigned long DELAY_RECEBER_LORA = 200;
const unsigned long TEMPO_ENVIAR_DADO_GATEWAY = 30000;
const unsigned long TEMPO_ESPERANDO_DADO_GATEWAY = 30000;
const int TENTATIVAS_ENVIO_GATEWAY = 3;

String tokenSensor;
String idSensor;
String umidade;
String longitude;
String latitude;
String leituraRecebida;
//=========================================

//================{Funções}================
//Dados
void esvaziaVariaveis();
void atualizaLeituraRecebida();

//Comunicação
bool enviaDadoNRF(String dado, unsigned long tempoTentandoEnviar);
String recebeDadoNRF(unsigned long tempoEsperandoDado);
String enviaDadoJsonNRF(String dado, unsigned long tempoTentandoEnviar);
bool recebeDadoJsonNRF(int indiceDado, unsigned long tempoEsperandoDado);
bool validaDadoJsonRecebido(String dadoJson, int indiceDado);
void enviaDadoLora(String dado);
String recebeDadoLora(unsigned long tempoEspera);
bool enviaDadoJsonLoRa (String dado, unsigned long tempoTentandoEnviar, int tentativas);
String recebeDadoJsonLora(unsigned long tempoEsperandoDado);
//=========================================

void setup() {
  Serial.begin(9600);
  lora.begin(9600);
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
  delay(DELAY_ESTABILIZAR);
}

void loop() {
  Serial.println("============={Loop - ROOT}============");
  esvaziaVariaveis();
  if (recebeDadoJsonNRF(0, TEMPO_ESPERANDO_DADO_SENSOR)) {
    Serial.println("Token Recebido!!!");
    if (tokenSensor == TOKEN) {
      Serial.println("Token Valido");
      if (enviaDadoNRF(AUTORIZADO, TEMPO_ENVIANDO_AUTORIZACAO)) {
        bool dadosJsonRecebidos = false;
        for (int i = 1; i <= 4; i++) {
          dadosJsonRecebidos = recebeDadoJsonNRF(i, TEMPO_ESPERANDO_DADO_SENSOR);
          if (!dadosJsonRecebidos) {
            i = 5;
          }
        }
        if (dadosJsonRecebidos) {
          atualizaLeituraRecebida();
          Serial.println("Todos os Dados Foram Recebidos Com Exito, ID Sensor: " + String(idSensor));
          Serial.println("Leitura: " + leituraRecebida);
          int idSensorInt = idSensor.toInt();
          Serial.println("Id Sensor: " + String(idSensorInt));
          bool leituraEnvidaGateway = enviaDadoJsonLora(leituraRecebida, TEMPO_ENVIAR_DADO_GATEWAY, TENTATIVAS_ENVIO_GATEWAY);
          if (leituraEnvidaGateway) {
            if (idSensorInt == 0) {
              String idCadastroSensor = recebeDadoJsonLora(TEMPO_ESPERANDO_DADO_GATEWAY);
              Serial.println("Id Sensor cadastro: " + idCadastroSensor);
              enviaDadoNRF(idCadastroSensor, TEMPO_ENVIAR_DADO_SENSOR);
            }
          } else {
            Serial.println("FALHA ao Enviar Leitura ao Gateway");
          }
        }
      }
    }
    Serial.println("======================================");
  }
}
void esvaziaVariaveis() {
  tokenSensor = "";
  idSensor = "";
  umidade = "";
  longitude = "";
  latitude = "";
  leituraRecebida = "";
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
    } else {
      Serial.println("Erro ao Enviar");
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
    Serial.println("Dado Json Recebido: " + dadoJsonRecebido);
    if (dadoJsonRecebido != INSUCESSO) {
      unsigned long tempoEnviarResposta = tempoEsperandoDado / 5;
      if (validaDadoJsonRecebido(dadoJsonRecebido, indiceDado)) {
        enviaDadoNRF(SUCESSO, tempoEnviarResposta);
        dadoJsonFoiRecebido = true;
        momentoInicial -= tempoEsperandoDado;
      } else {
        enviaDadoNRF(INSUCESSO, tempoEnviarResposta);
      }
    }
  }
  return dadoJsonFoiRecebido;
}

bool validaDadoJsonRecebido(String dadoJson, int indiceDado) {
  bool dadoValido = false;
  DeserializationError err = deserializeJson(doc, dadoJson);
  if (!err) {
    Serial.println("Dado Json Válido");
    if (indiceDado == 0) {
      const char* tokenSensorChar = doc["t"];
      tokenSensor = tokenSensorChar;
      int dadoEsperado = tokenSensor.indexOf("t");
      if (dadoEsperado == 0) {
        tokenSensor = extraiDadoNRF(tokenSensor);
        Serial.println("Mensagem limpa: " + tokenSensor);
        dadoValido = true;
      }
    }

    if (indiceDado == 1) {
      const char* idSensorChar = doc["id"];
      idSensor = idSensorChar;
      int dadoEsperado = idSensor.indexOf("id");
      if (dadoEsperado == 0) {
        idSensor = extraiDadoNRF(idSensor);
        Serial.println("Mensagem limpa: " + idSensor);
        dadoValido = true;
      }
    }

    if (indiceDado == 2) {
      const char* umidadeChar = doc["umi"];
      umidade = umidadeChar;
      int dadoEsperado = umidade.indexOf("umi");
      if (dadoEsperado == 0) {
        umidade = extraiDadoNRF(umidade);
        Serial.println("Mensagem limpa: " + umidade);
        dadoValido = true;
      }
    }

    if (indiceDado == 3) {
      const char* longitudeChar = doc["lon"];
      longitude = longitudeChar;
      int dadoEsperado = longitude.indexOf("lon");
      if (dadoEsperado == 0) {
        longitude = extraiDadoNRF(longitude);
        Serial.println("Mensagem limpa: " + longitude);
        dadoValido = true;
      }
    }

    if (indiceDado == 4) {
      const char* latitudeChar = doc["lat"];
      latitude = latitudeChar;
      int dadoEsperado = latitude.indexOf("lat");
      if (dadoEsperado == 0) {
        latitude = extraiDadoNRF(latitude);
        Serial.println("Mensagem limpa: " + latitude);
        dadoValido = true;
      }
    }
  }
  return dadoValido;
}

String extraiDadoNRF(String dado) {
  String dadoExtraido = "";
  int inicioMensagem = dado.indexOf("[");
  int fimMensagem = dado.indexOf("]");
  inicioMensagem++;
  fimMensagem--;
  for (int i = inicioMensagem; i <= fimMensagem; i++) {
    dadoExtraido += dado[i];
  }
  return dadoExtraido;
}

void atualizaLeituraRecebida() {
  leituraRecebida = "{\"t\":\"" + TOKEN + "\",\"id\":" + idSensor + ",\"umi\":" + umidade + ",\"lon\":" + longitude + ",\"lat\":" + latitude + "}";
}

void enviaDadoLora (String dado) {
  dado = "(" + dado + ")";
  lora.print(dado);
}

String recebeDadoLora(unsigned long tempoEsperandoDado) {
  String dadoRecebido = INSUCESSO;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
    if (lora.available() > 1) {
      dadoRecebido = lora.readString();
      String dadoExtraido = "";
      int inicioMensagem = dadoRecebido.indexOf("(");
      int fimMensagem = dadoRecebido.indexOf(")");
      inicioMensagem++;
      fimMensagem--;
      for (int i = inicioMensagem; i <= fimMensagem; i++) {
        dadoExtraido += dadoRecebido[i];
      }
      dadoRecebido = dadoExtraido;
      momentoInicial -= tempoEsperandoDado;
    }
    delay(20);
  }
  return dadoRecebido;
}

String recebeDadoJsonLora(unsigned long tempoEsperandoDado) {
  String dadoRecebido = INSUCESSO;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
    if (lora.available() > 1) {
      dadoRecebido = lora.readString();
      Serial.println("Dado Recebido: " + dadoRecebido);
      String dadoExtraido = "";
      int inicioMensagem = dadoRecebido.indexOf("(");
      int fimMensagem = dadoRecebido.indexOf(")");
      inicioMensagem++;
      fimMensagem--;
      for (int i = inicioMensagem; i <= fimMensagem; i++) {
        dadoExtraido += dadoRecebido[i];
      }
      dadoRecebido = dadoExtraido;
      momentoInicial -= tempoEsperandoDado;
      enviaDadoLora(SUCESSO);
    }
    delay(20);
  }
  return dadoRecebido;
}

bool enviaDadoJsonLora (String dado, unsigned long tempoTentandoEnviar, int tentativas) {
  bool dadoJsonFoiEnviado = false;
  unsigned long tempoPorTentativa = tempoTentandoEnviar / tentativas;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoTentandoEnviar) {
    enviaDadoLora(dado);
    String dadoFoiEnviado = recebeDadoLora(tempoPorTentativa);
    Serial.println("Status de Envio Dado Json: " + dadoFoiEnviado);
    if (dadoFoiEnviado == SUCESSO) {
      dadoJsonFoiEnviado = true;
      momentoInicial -= tempoTentandoEnviar;
    } else {
      delay(DELAY_ENVIAR_LORA);
    }
  }
  return dadoJsonFoiEnviado;
}
