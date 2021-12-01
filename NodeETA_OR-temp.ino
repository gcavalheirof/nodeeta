#include <DS18B20.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>
#define pin_led_v 33  //LED WIFI
#define pin_led_a 26  //LED MQTT

DS18B20 ds(19);
WebServer server(80);

const char* ssid = "iot";
const char* password = "jt72M8vP";
const char* mqtt_server = "10.116.88.10";
const char* mqtt_user = "broker-eta";
const char* mqtt_pass = "Broker-eta@nuplam";
hw_timer_t *timer = NULL;
long lastMsg = 0;
bool muda_led = false;
char temp[50];

IPAddress local_IP(10, 116, 88, 32); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
IPAddress gateway(10, 116, 88, 1); //GATEWAY DE CONEXÃO (ALTERE PARA O GATEWAY DO SEU ROTEADOR)
IPAddress subnet(255, 255, 255, 0); //MASCARA DE REDE

WiFiClient espClient;
PubSubClient client(espClient);

void IRAM_ATTR resetModule(){
  ets_printf("(watchdog) reiniciar\n"); //imprime no log
  esp_restart(); //reinicia o chip
}

void configureWatchdog()
{
  timer = timerBegin(0, 80, true); //timerID 0, div 80
  //timer, callback, interrupção de borda
  timerAttachInterrupt(timer, &resetModule, true);
  //timer, tempo (us), repetição
  timerAlarmWrite(timer, 5000000, true);
  timerAlarmEnable(timer); //habilita a interrupção //enable interrupt
}

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

  WiFi.begin(ssid, password);
  //WiFi.config(local_IP, gateway, subnet);

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    if(muda_led)
    digitalWrite(pin_led_v, HIGH);
    else
    digitalWrite(pin_led_v, LOW);
    muda_led = !muda_led;
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
  digitalWrite(pin_led_v, HIGH);
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

    if(muda_led)
    digitalWrite(pin_led_a, HIGH);
    else
    digitalWrite(pin_led_a, LOW);

    muda_led = !muda_led;
    
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe("/eta/or_temp");
      digitalWrite(pin_led_a, HIGH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(400);
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(33, OUTPUT);
  pinMode(26, OUTPUT);
  digitalWrite(pin_led_a, HIGH);
  digitalWrite(pin_led_v, HIGH);
  delay(1000);
  digitalWrite(pin_led_a, LOW);
  digitalWrite(pin_led_v, LOW);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  
  //MQTT
  client.setServer(mqtt_server, 1890);
  client.setCallback(callback);

  //OTA
  server.on("/", []() {
  server.send(200, "text/plain", "Hi! I am ESP_OR-temp.");
  });
  ElegantOTA.begin(&server, "Esp-OR-temp", "Esp-or-temp@nuplam");    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  //WATCHDOG
  configureWatchdog();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    if(muda_led)
    digitalWrite(pin_led_v, HIGH);
    else
    digitalWrite(pin_led_v, LOW);
    muda_led = !muda_led;
    Serial.print(".");
  }
  
  long now = millis();
  if (now - lastMsg > 5000) {
    Serial.println(ds.getTempC());
    snprintf (temp, 75, "%lf", ds.getTempC());
    //String sinal = "sinal_OR_temp: " + String(WiFi.RSSI());
    //client.publish("/eta/sinal_wifi", sinal); //envia força do sinal wifi
    client.publish("/eta/or_temp", temp);
    lastMsg = now;
  }

   timerWrite(timer, 0); //reseta temporizador WD
   server.handleClient();
   delay(50);
}
