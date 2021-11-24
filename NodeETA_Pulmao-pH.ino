//MODBUS:   A - BRANCO
//          B - VERDE



#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>
#include <stdlib.h>
#include <FastCRC.h>
#define SSerialTxControl   33   //RS485 Direction control
#define RS485Transmit    HIGH
#define RS485Receive     LOW
WebServer server(80);

const char* ssid = "iot";
const char* password = "jt72M8vP";

const char* mqtt_server = "10.116.88.10";
const char* mqtt_user = "broker-eta";
const char* mqtt_pass = "Broker-eta@nuplam";
int timer_reset = 0;
long lastMsg = 0;
int change_poll = 0;
const char* menosum = "-1";
bool mudarSP = false;

union {
   byte b[4];
   float fval;
}u;

byte msg2[10];
int i = 0;
float fval_sp;
byte b_sp[4];

IPAddress local_IP(10, 116, 88, 31 ); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR. EX: 192.168.1.110 **** ISSO VARIA, NO MEU CASO É: 192.168.0.175
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

void FloatToHex(float f, byte* hex){
  byte* f_byte = reinterpret_cast<byte*>(&f);
  memcpy(hex, f_byte, 4);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    message += (char)payload[i];
  }
  
  if(strcmp(topic, "/eta/envia_sp") == 0){
    mudarSP = true;
    fval_sp = message.toFloat();
    FloatToHex(fval_sp, b_sp);
  }
  Serial.println("");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe("/eta/ph");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      timer_reset++;
      if(timer_reset == 6) ESP.restart();
    }
  }
}

void consulta_ph()
{  
   digitalWrite(SSerialTxControl, RS485Transmit);  //LER PH ENDEREÇO 0550
   Serial2.write((byte)0x03);
   Serial2.write((byte)0x03);
   Serial2.write((byte)0x05);
   Serial2.write((byte)0x50);
   Serial2.write((byte)0x00);
   Serial2.write((byte)0x02);
   Serial2.write((byte)0xC5);
   Serial2.write((byte)0x34);
   delay(30);
   digitalWrite(SSerialTxControl, RS485Receive);
   resposta(false);
} 

void escrita_sp1_pH()
{
  FastCRC16 CRC16;
   digitalWrite(SSerialTxControl, RS485Transmit);  //ESCREVER SP1 ENDEREÇO 0200
   uint8_t packet [11] = {0x03, 0x10, 0x02, 0x00, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
   packet[7] = b_sp[3];
   packet[8] = b_sp[2];
   unsigned int crcXmodem = CRC16.modbus(packet, 9);
   packet[10] = highByte(crcXmodem);
   packet[9] = lowByte(crcXmodem);
   Serial2.write(packet[0]);
   Serial2.write(packet[1]);
   Serial2.write(packet[2]);
   Serial2.write(packet[3]);
   Serial2.write(packet[4]);
   Serial2.write(packet[5]);
   Serial2.write(packet[6]);
   Serial2.write(packet[7]);
   Serial2.write(packet[8]);
   Serial2.write(packet[9]);
   Serial2.write(packet[10]);
   delay(30);
   digitalWrite(SSerialTxControl, RS485Receive);
}

void escrita_sp1_mA()
{
   FastCRC16 CRC16;
   digitalWrite(SSerialTxControl, RS485Transmit);  //ESCREVER SP1 ENDEREÇO 0200
   uint8_t packet [11] = {0x03, 0x10, 0x04, 0x30, 0x00, 0x02, 0x04, 0x00, 0x00, 0x00, 0x00};
   packet[7] = b_sp[3];
   packet[8] = b_sp[2];
   unsigned int crcXmodem = CRC16.modbus(packet, 9);
   packet[9] = highByte(crcXmodem);
   packet[10] = lowByte(crcXmodem);
   Serial2.write(packet[0]);
   Serial2.write(packet[1]);
   Serial2.write(packet[2]);
   Serial2.write(packet[3]);
   Serial2.write(packet[4]);
   Serial2.write(packet[5]);
   Serial2.write(packet[6]);
   Serial2.write(packet[7]);
   Serial2.write(packet[8]);
   Serial2.write(packet[9]);
   Serial2.write(packet[10]);
   delay(30);
   digitalWrite(SSerialTxControl, RS485Receive);
}

void consulta_sp1()
{  
   digitalWrite(SSerialTxControl, RS485Transmit);  //LER SETPOINT1 ENDEREÇO 0200
   Serial2.write((byte)0x03);
   Serial2.write((byte)0x03);
   Serial2.write((byte)0x02);
   Serial2.write((byte)0x00);
   Serial2.write((byte)0x00);
   Serial2.write((byte)0x02);
   Serial2.write((byte)0xC4);
   Serial2.write((byte)0x51);
   //Serial.println("Consulta SP1");
   delay(30);
   digitalWrite(SSerialTxControl, RS485Receive);
   resposta(true);
} 

void resposta(bool ph){
  i=0;
  if(Serial2.available()){
      timer_reset = 0;
      while (Serial2.available())
      {
          byte msg = Serial2.read();
          msg2[i] = msg;
          i++;
       }
       if(msg2[0] == 0x03 && msg2[1] == 0x03 && ph){ // ID=03 | FC=03 | PUB no pH
        u.b[0] = msg2[6];
        u.b[1] = msg2[5];
        u.b[2] = msg2[4];
        u.b[3] = msg2[3];
        Serial.print("SP1: ");
        Serial.println(u.fval, 4);
        String aux = String(u.fval);
        client.publish("/eta/ph", aux.c_str());
       }else if(msg2[0] == 0x03 && msg2[1] == 0x03 && !ph){ // ID=03 | FC=03 | PUB no SP
        u.b[0] = msg2[6];
        u.b[1] = msg2[5];
        u.b[2] = msg2[4];
        u.b[3] = msg2[3];
        Serial.print("pH: ");
        Serial.println(u.fval, 4);
        String aux = String(u.fval);
        client.publish("/eta/th_sp1", aux.c_str());

        //junto com o ph, enviar o sinal do wifi:
        String sinal = "sinal_pH: " + String(WiFi.RSSI());
        client.publish("/eta/sinal_wifi", sinal.c_str()); //envia força do sinal wifi
       }
    }
    else{
         client.publish("/eta/ph", menosum);
         timer_reset++;
    }
}
void setup()
{
  // Start the builtin serial port, probably to Serial Monitor
  Serial.begin(9600);
  Serial2.begin(9600);
  WiFi.mode(WIFI_STA);
  setup_wifi();
  
  //MQTT
  client.setServer(mqtt_server, 1890);
  client.setCallback(callback);

  //OTA
  server.on("/", []() {
  server.send(200, "text/plain", "Hi! I am ESP_PH.");
  });
  ElegantOTA.begin(&server, "Esp-PH", "Esp-ph@nuplam");    // Start ElegantOTA
  server.begin();
  Serial.println("HTTP server started");

  
  //RS485
  pinMode(SSerialTxControl, OUTPUT);  
  digitalWrite(SSerialTxControl, RS485Receive);  // Init Transceiver
  delay(2000);
}

void loop()   
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  
  if (now - lastMsg > 2500) {
    
    if(change_poll == 0)
      consulta_ph();
    else if(change_poll == 1)
      consulta_sp1();

    lastMsg = now;
    change_poll++;
    if(change_poll > 1) change_poll = 0;
  }
  
  /*client.subscribe("/eta/envia_sp");
  if(mudarSP){
    escrita_sp1_pH();
    escrita_sp1_mA();
    mudarSP = false;
  }*/
    
  if (timer_reset == 20){
    ESP.restart();
  }
  
  server.handleClient();
  delay(50);
}
