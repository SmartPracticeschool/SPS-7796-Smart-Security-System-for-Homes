#include<ESP8266WiFi.h>
#include<PubSubClient.h>
#include "DHT.h"
#define DHTPIN D2
#define DHTTYPE DHT11
#define MQ2pin (0)
DHT dht (DHTPIN, DHTTYPE);
float t,s;
Servo servom;
int angle = 0;
int h;
String command;
String data="";
void callback(char* topic, byte* payload, unsigned int payloadLength);
const char* ssid = "Wifi";
const char* password = "very101Secure";
#define ORG "2twfpy"
#define DEVICE_TYPE "MyDevice"
#define DEVICE_ID "HomeIOT"
#define TOKEN "&j6U@W80ipoFTBOuSS"
#define led1 D0
const char publishTopic[] = "iot-2/evt/Data/fmt/json";
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char topic[] = "iot-2/cmd/home/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_Type ":" DEVICE_ID;
WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
int publishInterval = 5000;
long lastPublishMillis;
void publishData();

void setup(){
  Serial.begin(115200);
  Serial.println();
  pinMode(led1, OUTPUT);
  servom.attach(9);
  dht.begin();
  wifiConnect();
  mqttConnect();
}

void loop(){
  if(millis() - lastPublishMillis > publishInterval){
    publishData();
    lastPublishMillis = millis();  
  }
  if(!client.loop()){
    mqttConnect();
  }
}

void wifiConnect(){
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.print("WiFi connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttConnect(){
  if(!client.connected()){
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while(!client.connect(clientId, authMethod, token)){
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}

void initManagedDevices(){
  if(client.subscribe(topic)){
    Serial.println("Subscribe to cmd OK");
  } else{
    Serial.println("Subscribe to cmd FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength){
  Serial.print("callback invoked for topic: ");
  Serial.println(topic);
  for(int i=0; i< payloadLength; i++){
    command+= (char)payload[i];
  }
  Serial.print("data: "+ command);
  control_func();
  command="";
}

void control_func(){
  if(command=="lightoff"){
    digitalWrite(led1, LOW);
    Serial.println("Lights were switched Off.");  
  }
  else if(command == "lighton"){
    digitalWrite(led1,HIGH);
    Serial.println("Lights were switched On.");  
  }
  else if(command == "dooropen"){
    if(angle == 0){
      for(angle = 0; angle <= 180; angle +=5){
        servom.write(angle);
        delay(5);  
      } 
    }
  }
  else if(command == "doorclose"){
    if(angle == 180){
      for(angle = 180; angle >= 1; angle -=5){
        servom.write(angle);
        delay(5);
      }
    }
  }
  else{
    Serial.println("No commands subscribed");  
  }

  
}

void publishData(){
  h = dht.readHumidity();
  t = dht.readTemperature();
  s = analogRead(MQ2pin);
  if(isnan(h) || isnan(t)){
    Serial.println("Failed to read the DHT sensor.");
    return;  
  }
  if(isnan(s)){
    Serial.println("Failed to read the MQ-2 sensor.");
    return;  
  }
  String payload = "{\"d\":{\"temperature\":";
  payload += t;
  payload += ",""\"humidity\":";
  payload += h;
  payload += ",""\"smoke\":";
  payload += s;
  payload += "}}";
  Serial.print("\n");
  Serial.print("Sending payload: ");Serial.println(payload);
  if(client.publish(publishTopic, (char*) payload.c_str())){
    Serial.println("Publish OK");
  }
  else{
    Serial.println("Publish FAILED");
  }
}