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
          Serial.println("Leitura: "+leituraRecebida);
          if (idSensor == 0) {            
            Serial.print("Cadastrar Sensor: ");
            bool dadosJsonEnviadosGateway = enviaDadoJsonLoRa(leituraRecebida, TEMPO_ENVIAR_DADO_GATEWAY, TENTATIVAS_ENVIO_GATEWAY);
            if (dadosJsonEnviadosGateway) {
              if (recebeDadosJsonLoRa(TEMPO_ESPERANDO_DADO_GATEWAY)) {
                Serial.println("Sucesso, id cadastrado: " + String(idCadastroSensor));
                if (idCadastroSensor > 0) {
                  enviaDadoJsonNRF("{\"id\":"+String(idCadastroSensor)+"}", TEMPO_ENVIAR_DADO_SENSOR);
                }
              } else {
                Serial.println("ERRO: Não Foi Recebido Resposta");
              }
            }else {
              Serial.println("ERRO: Não Foi Recebido Resposta");
            }
          } else {
            Serial.print("Salvar Leitura: ");
            bool leituraSalva = enviaDadoJsonLoRa(leituraRecebida, TEMPO_ENVIAR_DADO_GATEWAY, TENTATIVAS_ENVIO_GATEWAY);
            if(leituraSalva) {
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
  leituraRecebida = "{\"t\":\"" + TOKEN + "\",\"id\":" + String(idSensor) + ",\"umi\":" + String(umidade, 2) + ",\"lon\":" + String(longitude, 14) + ",\"lat\":" + String(latitude, 14) + "}";
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
  DeserializationError err = deserializeJson(doc, dadoJson);
  if (!err) {
    if (indiceDado == 0) {
      const char* tokenSensorChar = doc["t"];
      tokenSensor = tokenSensorChar;
      if (tokenSensor != "") {
        dadoValido = true;
      }
    } else if (indiceDado == 1) {
      idSensor = doc["id"];
      if (idSensor >= 0) {
        dadoValido = true;
      }
    } else if (indiceDado == 2) {
      umidade = doc["umi"];
      if (umidade >= 0) {
        dadoValido = true;
      }
    } else if (indiceDado == 3) {
      longitude = doc["lon"];
      if (longitude != 0) {
        dadoValido = true;
      }
    } else if (indiceDado == 4) {
      latitude = doc["lat"];
      if (latitude != 0) {
        dadoValido = true;
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
      //dadosJsonValidos = validaDadoJsonRecebidoLoRa(dadoJsonRecebido);
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
