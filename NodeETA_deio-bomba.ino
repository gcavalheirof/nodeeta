#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>
#include <EmonLib.h>  // biblioteca do sensor de corrente

WebServer server(80);

const char* ssid = "iot";
const char* password = "jt72M8vP";
const char* mqtt_server = "10.116.88.10";
const char* mqtt_user = "broker-eta";
const char* mqtt_pass = "Broker-eta@nuplam";
long lastMsg = 0;
int timer_reset = 0;
EnergyMonitor emon;  // cria o objeto p/ interagir com o sensor

IPAddress local_IP(10, 116, 88, 33); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
IPAddress gateway(10, 116, 88, 1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255, 255, 255, 0); //MASCARA DE REDE

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
 // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(ssid, password);    // canal 9
  //WiFi.config(local_IP, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  randomSeed(micros());
    
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe("/eta/deio_bomba");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      timer_reset++;
      if(timer_reset == 20) ESP.restart();
    }
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  
  //MQTT
  client.setServer(mqtt_server, 1890);
  client.setCallback(callback);

  //OTA
  server.on("/", []() {
  server.send(200, "text/plain", "Hi! I am ESP_deio-bomba.");
  });
  ElegantOTA.begin(&server, "ESP_deio-bomba", "Esp_deio-bomba@nuplam");    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  //SENSOR DE CORRENTE
  emon.current(15, 29);   //D4 -> entrada analogica  42-> calibracao
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();
  if (now - lastMsg > 5000) {
   
  double irms = emon.calcIrms(1480);     // funcao que le a corrente
  String corrente = String(irms);
    client.publish("/eta/deio_bomba", corrente.c_str());   //quebra string em caractere
    lastMsg = now;
  }
}
