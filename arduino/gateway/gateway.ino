#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//=================={Json}=================
StaticJsonDocument<256> doc;
//=========================================

//=================={Radio}================
SoftwareSerial lora(D2, D3); // TX, RX
//=========================================

//========={Variaveis e Constantes}========
const String TOKEN = "1.1603836354";
const String SUCESSO = "sucesso";
const String INSUCESSO = "insucesso";
const String CADASTRO = "http://192.168.100.6/repositorio/tcc-sensor-de-umidade/server/sensor/";
const String SALVALEITURA = "http://192.168.100.6/repositorio/tcc-sensor-de-umidade/server/leitura/";
const char* ssid = "Paula oi fibra";
const char* password = "abcdy098";

const unsigned long DELAY_ENVIAR_LORA = 200;
const unsigned long TEMPO_ENVIAR_DADO_ROOT = 30000;
const unsigned long TEMPO_ESPERANDO_DADO_ROOT = 30000;
const int TENTATIVAS_ENVIO_ROOT = 3;

String tokenRecebido;
int idSensor;
int idSensorCadastro;
float umidade;
float longitude;
float latitude;

String leituraRecebida;
//=========================================

//================{Funções}================
//Dados
void esvaziaVariaveis();
bool atualizaDados();

//Comunicação
void enviaDadoLora(String dado);
String recebeDadoLora(unsigned long tempoEspera);
bool enviaDadoJsonLora (String dado, unsigned long tempoTentandoEnviar, int tentativas);
String recebeDadoJsonLora(unsigned long tempoEsperandoDado);
String requisicaoHTTP(String link, String dado, bool cadastrar);
//=========================================
void setup() {
  Serial.begin(9600);
  delay(1000);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("Conectado com exito, meu IP é: ");
  Serial.println(WiFi.localIP());
  lora.begin(9600);
}

void loop() {
  Serial.println("============={LOOP GATEWAY}=============");
  esvaziaVariaveis();
  leituraRecebida = recebeDadoJsonLora(TEMPO_ESPERANDO_DADO_ROOT);
  Serial.println("Leitura Recebida: " + leituraRecebida);
  bool dadosAtualizados = atualizaDados();
  if (dadosAtualizados) {
  Serial.println("Dados Válidos!");
  Serial.println("Id: " + String(idSensor));
    if (idSensor == 0) {
      String dadosSensor = "{\"lon\":" + String(longitude) + ",\"lat\":" + String(latitude) + ",\"umi\":" + String(umidade) + "}";
      String respostaServer = requisicaoHTTP(CADASTRO, dadosSensor, true);
      Serial.println("Resposta do Servidor: " + respostaServer);
      enviaDadoJsonLora(respostaServer, TEMPO_ENVIAR_DADO_ROOT, TENTATIVAS_ENVIO_ROOT);
    } else {
      String dadosSensor = "{\"id\":" + String(idSensor) + ",\"lon\":" + String(longitude) + ",\"lat\":" + String(latitude) + ",\"umi\":" + String(umidade) + "}";
      String respostaServer = requisicaoHTTP(SALVALEITURA, dadosSensor, false);
      Serial.println("Resposta do Servidor: " + respostaServer);
    }
  }
  Serial.println("========================================");
}

void esvaziaVariaveis() {
  leituraRecebida = "";
  tokenRecebido = "";
  idSensor = -1;
  idSensorCadastro = 0;
  umidade = -1;
  longitude = 0;
  latitude = 0;
}

String requisicaoHTTP(String link, String dado, bool cadastrar) {
  String respostaRequisicao = INSUCESSO;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String dado_a_enviar;
    if (cadastrar) {
      dado_a_enviar = "dadosSensor=" + dado;  
    } else {
      dado_a_enviar = "leitura=" + dado;  
    }
    http.begin(link);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http.addHeader("TokenRede", TOKEN);

    int codigo_resposta = http.POST(dado_a_enviar);

    if (codigo_resposta > 0) {
      Serial.println("Código HTTP: " + String(codigo_resposta));

      if (codigo_resposta == 200) {
        String corpo_resposta = http.getString();
        Serial.println("Resposta do Servidor: ");
        Serial.println(corpo_resposta);
        respostaRequisicao = corpo_resposta;
      }
    }
  }
  return respostaRequisicao;
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
      enviaDadoLora("sucesso");
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

bool atualizaDados() {
  bool leituraValida = true;
  DeserializationError err = deserializeJson(doc, leituraRecebida);
  
  if (!err) {
    const char* tokenRecebidoChar = doc["t"];
    tokenRecebido = tokenRecebidoChar;
    
    idSensor  = doc["id"];
    umidade   = doc["umi"];
    longitude = doc["lon"];
    latitude  = doc["lat"];

    if (tokenRecebido != TOKEN or idSensor < 0 or umidade < 0 or longitude == 0 or latitude == 0) {
      leituraValida = false;
    } 
  } else {
    leituraValida = false;
  }
  
  if (leituraValida == false) {
    esvaziaVariaveis();
  }
  
  return leituraValida;
}
