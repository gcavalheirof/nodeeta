#include <PubSubClient.h>
#include <EmonLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ElegantOTA.h>

#define PIN_SENSOR A0

int CONTATO_ENXAGUE = D6;
int CONTATO_PROD = D7;
int CONTATO_BOMBA = D8;
const char* estado = "";

EnergyMonitor emon;	// Objeto da lib EmonLib
ESP8266WebServer server(80);    // Objeto da lib ESP8266WebServer

const char* ssid = "iot";   // Identificador da rede
const char* password = "******";    // Senha da rede

const char* mqtt_server = "10.116.88.10";	// IP do broker

IPAddress local_IP(10, 116, 88, 30);// IP fixo para o ESP
IPAddress gateway(10, 116, 88, 1);  // Gateway de conexão
IPAddress subnet(255, 255, 255, 0); // Máscara de rede

WiFiClient espClient;   // Objeto da lib WifiCliente
PubSubClient client(espClient); // Objeto da lib PubSubClient
long lastMsg = 0;   // Tempo da última mensagem
char corrente[50];  // String para envio MQTT


// Funcao de configuracao e conexao a rede WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando com ");
  Serial.println(ssid);

  // Configura IP estático
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao configurar");
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("endereço de IP: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}


// Funcao de exibicao da mensagem recebida via serial
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Função de reconeccao com broker MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "NodeETA_OR"; // ID do dispositivo

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
      client.subscribe("/eta/or/bomba_alta");
    } else {
      Serial.print("Falhou, codigo de retorno =");
      Serial.print(client.state());
      Serial.println(" Tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200); // Inicia porta serial com taxa 115200
  setup_wifi(); // Configuração de rede
  client.setServer(mqtt_server, 1890);  // Define o broker MQTT
  client.setCallback(callback); // Define função callback
  emon.current(PIN_SENSOR, 42); // Definição do pino de leitura 

  // Start no server ElegantOTA
  server.on("/", []() {
    server.send(200, "text/plain", "ESP8266 da Osmose Reversa.");
  });
  ElegantOTA.begin(&server, "Esp-OR", "Esp-or@nuplam");    
  server.begin();
  Serial.println("Server HTTP iniciado");

  pinMode(CONTATO_ENXAGUE, INPUT);  // Contato enxágue
  pinMode(CONTATO_PROD, INPUT); // Contato automático
  pinMode(CONTATO_BOMBA, INPUT);    // Contato liga/desliga bomba
}

void loop() {

  // Conexao com o broker MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Leitura da corrente e convercao em rms
  double irms = emon.calcIrms(1480);
  delay(200);
  
  // Atualizacao de tempo
  long now = millis();
  
  // Amostras a cada 2 segundos
  if (now - lastMsg > 2000) {
    // Estados do processo
	// Producao = 1
	// Sanitização = 2 
	// Enxágue = 3
	// Manutenção = 4
    if (digitalRead(CONTATO_BOMBA)) {
      if (digitalRead(CONTATO_ENXAGUE) == HIGH)
      {
        estado = "3";
      }
      if (digitalRead(CONTATO_PROD) == HIGH)
      {
        estado = "1";
      }
      if (!digitalRead(CONTATO_ENXAGUE) &&
		  !digitalRead(CONTATO_PROD)) {
        estado = "2";
      }
    }
    if (digitalRead(CONTATO_BOMBA) == LOW) {
      estado = "4";
    }

    // Atualizacao do tempo
    lastMsg = now;
	
	// Atualizacao da string
    snprintf (corrente, 75, "%lf", irms);

	// Para visualizacao dos dados via serial
	Serial.println(corrente);
    Serial.println(estado);

    // Publicacao das variaveis no broker
    client.publish("/eta/or/bomba_alta", corrente);
    client.publish("/eta/or/estado", estado);
    
	// Atualizacao webserver
	server.handleClient();
  }
}
