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
const unsigned long DELAY_ESTABILIZAR = 5000;

const unsigned long DELAY_ENVIAR_NRF = 200;
const unsigned long DELAY_RECEBER_NRF = 200;
const unsigned long TEMPO_ESPERANDO_DADO_SENSOR = 20000;
const unsigned long TEMPO_ENVIAR_DADO_SENSOR = 20000;
const unsigned long TEMPO_ENVIANDO_AUTORIZACAO = 20000;

const unsigned long DELAY_ENVIAR_LORA = 200;
const unsigned long DELAY_RECEBER_LORA = 200;
const unsigned long TEMPO_ENVIAR_DADO_GATEWAY = 30000;
const unsigned long TEMPO_ESPERANDO_DADO_GATEWAY = 30000;
const int TENTATIVAS_ENVIO_GATEWAY = 3;

String tokenSensor;
int idSensor;
float umidade;
float longitude;
float latitude;
String leituraRecebida;
int idCadastroSensor;

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
String extraiDadoNRF(String dado);
bool validaDadoJsonRecebido(String dadoJson, int indiceDado);
void enviaDadoLoRa (String dado);
String recebeDadoLoRa(unsigned long tempoEsperandoDado);
bool enviaDadoJsonLoRa (String dado, unsigned long tempoTentandoEnviar, int tentativas);
bool recebeDadosJsonLoRa(unsigned long tempoEsperandoDado);
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
    if (tokenSensor == TOKEN) {
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
          if (idSensor == 0) {
            Serial.print("Cadastrar Sensor: ");
            bool dadosJsonEnviadosGateway = enviaDadoJsonLoRa(leituraRecebida, TEMPO_ENVIAR_DADO_GATEWAY, TENTATIVAS_ENVIO_GATEWAY);
            if (dadosJsonEnviadosGateway) {
              if (recebeDadosJsonLoRa(TEMPO_ESPERANDO_DADO_GATEWAY)) {
                Serial.println("Sucesso, id cadastrado: " + String(idCadastroSensor));
                String idCadastroSensorStr = String(idCadastroSensor);
                enviaDadoJsonNRF("{\"id\":" + idCadastroSensorStr + "}", TEMPO_ENVIAR_DADO_SENSOR);
              } else {
                Serial.println("ERRO: Não Foi Recebido Resposta");
              }
            } else {
              Serial.println("ERRO: Não Foi Recebido Resposta");
            }
          } else {
            Serial.print("Salvar Leitura: ");
            bool leituraSalva = enviaDadoJsonLoRa(leituraRecebida, TEMPO_ENVIAR_DADO_GATEWAY, TENTATIVAS_ENVIO_GATEWAY);
            if (leituraSalva) {
              Serial.println("Leitura Salva Com Sucesso");
            } else {
              Serial.println("ERRO: Leitura não foi Enviada ao GATEWAY");
            }
          }
        } else {
          Serial.println("Os Dados Não Foram Recebidos Com Exito");
        }
      }
    }
  }
  Serial.println("======================================");
}

void atualizaLeituraRecebida() {
  String idSensorStr = String(idSensor);
  String umidadeStr = String(umidade, 2);
  String longitudeStr = String(longitude, 7);
  String latitudeStr = String(latitude, 7);
  leituraRecebida = "{\"t\":\"" + TOKEN + "\",\"id\":" + idSensorStr + ",\"umi\":" + umidadeStr + ",\"lon\":" + longitudeStr + ",\"lat\":" + latitudeStr + "}";
}

bool enviaDadoNRF(String dado, unsigned long tempoTentandoEnviar) {
  bool dadoFoiEnviado = false;
  char dadoChar[32];
  unsigned long momentoInicial = millis();

  dado.toCharArray(dadoChar, 32);

  while ((millis() - momentoInicial) <= tempoTentandoEnviar) {
    if (radio.write(&dadoChar, sizeof(dadoChar))) {
      dadoFoiEnviado = true;
      momentoInicial -= tempoTentandoEnviar;
    }
    delay(DELAY_ENVIAR_NRF);
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
    }
    delay(DELAY_RECEBER_NRF);
  }
  radio.stopListening();
  Serial.println("Dado Recebido do Sensor: " + String(dadoRecebido));
  return dadoRecebido;
}

String enviaDadoJsonNRF(String dado, unsigned long tempoTentandoEnviar) {
  String respostaAoEnviar = INSUCESSO;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoTentandoEnviar) {
    if (enviaDadoNRF(dado, tempoTentandoEnviar)) {
      respostaAoEnviar = recebeDadoNRF(tempoTentandoEnviar);
      if (respostaAoEnviar == SUCESSO) {
        momentoInicial -= tempoTentandoEnviar;
      }
    }
  }
  return respostaAoEnviar;
}

bool recebeDadoJsonNRF(int indiceDado, unsigned long tempoEsperandoDado) {
  bool dadoJsonFoiRecebido = false;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
    String dadoJsonRecebido = recebeDadoNRF(tempoEsperandoDado);
    if (dadoJsonRecebido != INSUCESSO) {
      if (validaDadoJsonRecebido(dadoJsonRecebido, indiceDado)) {
        if (enviaDadoNRF(SUCESSO, tempoEsperandoDado)) {
          dadoJsonFoiRecebido = true;
          momentoInicial -= tempoEsperandoDado;
        }
      } else {
        enviaDadoNRF(INSUCESSO, tempoEsperandoDado);
      }
    }
  }
  return dadoJsonFoiRecebido;
}

bool validaDadoJsonRecebido(String dadoJson, int indiceDado) {
  bool dadoValido = false;
  Serial.println("Dado pronto para ser validado");
  DeserializationError err = deserializeJson(doc, dadoJson);
  if (!err) {
    Serial.println("Dado Json Válido!");
    if (indiceDado == 0) {
      const char* tokenSensorChar = doc["t"];
      String tokenSensorStr = tokenSensorChar;
      int dadoEsperado = tokenSensorStr.indexOf("t");
      if (dadoEsperado == 0) {
        Serial.println("Dado Esperado");
        tokenSensorStr = extraiDadoNRF(tokenSensorStr);
        if (tokenSensorStr == TOKEN) {
          tokenSensor = tokenSensorStr;
          dadoValido = true;
        }
      }
    } else if (indiceDado == 1) {
      const char* idSensorChar = doc["id"];
      String idSensorStr = idSensorChar;
      int dadoEsperado = idSensorStr.indexOf("id");
      if (dadoEsperado == 0) {
        Serial.println("Dado Esperado");
        idSensorStr = extraiDadoNRF(idSensorStr);
        idSensor = idSensorStr.toInt();
        Serial.println("Dado extraido: " + String(idSensor));
        if (idSensor >= 0) {
          dadoValido = true;
        }
      }
    } else if (indiceDado == 2) {
      const char* umidadeChar = doc["umi"];
      String umidadeStr = umidadeChar;
      int dadoEsperado = umidadeStr.indexOf("umi");
      if (dadoEsperado == 0) {
        Serial.println("Dado Esperado");
        umidadeStr = extraiDadoNRF(umidadeStr);
        umidade = umidadeStr.toFloat();
        Serial.println("Dado extraido: " + umidadeStr);
        Serial.println("Dado extraido: " + String(umidade, 2));
        if (umidade >= 0) {
          dadoValido = true;
        }
      }
    } else if (indiceDado == 3) {
      const char* longitudeChar = doc["lon"];
      String longitudeStr = longitudeChar;
      int dadoEsperado = longitudeStr.indexOf("lon");
      if (dadoEsperado == 0) {
        Serial.println("Dado Esperado");
        longitudeStr = extraiDadoNRF(longitudeStr);
        longitude = longitudeStr.toFloat();
        Serial.println("Dado extraido: " + longitudeStr);
        Serial.println("Dado extraido: " + String(longitude, 10));
        if (longitude != 0) {
          dadoValido = true;
        }
      }
    } else if (indiceDado == 4) {
      const char* latitudeChar = doc["lat"];
      String latitudeStr = latitudeChar;
      int dadoEsperado = latitudeStr.indexOf("lat");
      if (dadoEsperado == 0) {
        Serial.println("Dado Esperado");
        latitudeStr = extraiDadoNRF(latitudeStr);
        latitude = latitudeStr.toFloat();
        Serial.println("Dado extraido: " + latitudeStr);
        Serial.println("Dado extraido: " + String(latitude, 10));
        if (latitude != 0) {
          dadoValido = true;
        }
      }
    } else if (indiceDado == 5) {
      idCadastroSensor = doc["id"];
      if (idCadastroSensor > 0) {
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

void enviaDadoLoRa (String dado) {
  dado = "[" + dado + "]";
  lora.print(dado);
}

String recebeDadoLoRa(unsigned long tempoEsperandoDado) {
  String dadoRecebido = INSUCESSO;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
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
        momentoInicial -= tempoEsperandoDado;
      } else {
        dadoRecebido = INSUCESSO;
      }
    }
    delay(20);
  }
  return dadoRecebido;
}

bool enviaDadoJsonLoRa (String dado, unsigned long tempoTentandoEnviar, int tentativas) {
  bool dadoJsonFoiEnviado = false;
  unsigned long tempoPorTentativa = tempoTentandoEnviar / tentativas;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoTentandoEnviar) {
    enviaDadoLoRa(dado);
    String dadoFoiEnviado = recebeDadoLoRa(tempoPorTentativa);
    if (dadoFoiEnviado == SUCESSO) {
      dadoJsonFoiEnviado = true;
      momentoInicial -= tempoTentandoEnviar;
    } else {
      delay(DELAY_ENVIAR_LORA);
    }
  }
  return dadoJsonFoiEnviado;
}

bool recebeDadosJsonLoRa(unsigned long tempoEsperandoDado) {
  bool dadosJsonValidos = false;
  unsigned long momentoInicial = millis();
  while ((millis() - momentoInicial) <= tempoEsperandoDado) {
    String dadoJsonRecebido = recebeDadoLoRa(tempoEsperandoDado);
    if (dadoJsonRecebido != INSUCESSO) {
      dadosJsonValidos = validaDadoJsonRecebido(dadoJsonRecebido, 5);
      if (dadosJsonValidos) {
        enviaDadoLoRa(SUCESSO);
        momentoInicial -= tempoEsperandoDado;
      } else {
        enviaDadoLoRa(INSUCESSO);
      }
    }
  }
  return dadosJsonValidos;
}

void esvaziaVariaveis () {
  tokenSensor = "";
  idSensor = -1;
  umidade = -1;
  longitude = 0;
  latitude = 0;
  leituraRecebida = "";
  idCadastroSensor = 0;
}
