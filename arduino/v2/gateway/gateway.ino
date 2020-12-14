#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

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
  esvaziaVariaveis();
  leituraRecebida = recebeDadoLora(15000);
  if (leituraRecebida != INSUCESSO) {
    bool dadosAtualizados = atualizaDados();
    if (dadosAtualizados) {
      Serial.println("============={Dados Válidos}=============");
      if (idSensor == 0) {
        String dadosSensor = "{\"lon\":" + String(longitude) + ",\"lat\":" + String(latitude) + ",\"umi\":" + String(umidade) + "}";
        String respostaServer = requisicaoHTTP(CADASTRO, dadosSensor, true);
        Serial.println(respostaServer);
        unsigned long tempoInicial = millis();
        unsigned long tempoTentandoEnviarId = 20000;
        DeserializationError err = deserializeJson(doc, respostaServer);
        const char* idRecebido = doc["idSensor"];
        String novoIdSensorJson = "[{\"id\":" + String(idRecebido) + "}]";
        while ((millis() - tempoInicial) <= tempoTentandoEnviarId) {
          enviaDadoLora(novoIdSensorJson);
          String respostaDeEnvio = recebeDadoLora(tempoTentandoEnviarId);
          if (respostaDeEnvio == SUCESSO) {
            tempoInicial -= tempoTentandoEnviarId;
          }
        }
      } else {
        enviaDadoLora("[sucesso]");
        String dadosSensor = "{\"id\":" + String(idSensor) + ",\"lon\":" + String(longitude) + ",\"lat\":" + String(latitude) + ",\"umi\":" + String(umidade) + "}";
        String respostaServer = requisicaoHTTP(SALVALEITURA, dadosSensor, false);
        Serial.println(respostaServer);
      }
      Serial.print("Id Sensor: ");
      Serial.println(idSensor);
      Serial.print("Umidade: ");
      Serial.println(umidade);
      Serial.print("Longitude: ");
      Serial.println(longitude, 7);
      Serial.print("Latitude: ");
      Serial.println(latitude, 7);
      Serial.println("==========================================");
    } else {
      Serial.println("Dados Inválidos");
      enviaDadoLora(INSUCESSO);
    }
  }
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

bool atualizaDados() {
  bool leituraValida = true;
  DeserializationError err = deserializeJson(doc, leituraRecebida);
  if (!err) {
    const char* tokenRecebidoChar = doc["t"];
    tokenRecebido = tokenRecebidoChar;
    idSensor = doc["id"];
    umidade = doc["umi"];
    longitude = doc["lon"];
    latitude = doc["lat"];

    if (tokenRecebido != TOKEN or
        idSensor < 0 or
        umidade < 0 or
        longitude == 0 or
        latitude == 0) {
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

void enviaDadoLora (String dado) {
  lora.print(dado);
}

String recebeDadoLora(unsigned long tempoEspera) {
  String dadoRecebido = "insucesso";
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

void esvaziaVariaveis() {
  leituraRecebida = "";
  tokenRecebido = "";
  idSensor = -1;
  idSensorCadastro = 0;
  umidade = -1;
  longitude = 0;
  latitude = 0;
}
