#include <Arduino.h>
#ifdef ESP32
  #include <WiFi.h>
  #include <ESP32Ping.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <PubSubClient.h>
#include "kmutnbLogin.h"

// Parameters
const char* ssid[] = {"@ENG-WIFI","Maka", "classroom_wifi", "@ENG-WIFI_328", "Wifi-ENG","@KMUTNB"}; 
const char* pwd[] = {NULL, NULL, NULL, NULL, NULL,NULL};
//String serverName = "https://security.eng.kmutnb.ac.th:1003/fgtauth";
//String web = "http://www.google.com"; //"http://google.com";
const int httpsPort = 443;
const int fgtauthPort = 1003;
String username = "xxx";
String password = "xxx";

AsyncWebServer server(80);
EthernetClient ethclient;
WiFiClient m5stack;

kmutnbLogin login(username, password); //

// global IP address from DigitalOcean cloud or else
#define mqtt_server "xxx.xxx.xxx.xxx" 
#define mqtt_port 1883 // default port 
#define mqtt_user "superman" // as broker setting
#define mqtt_password "1234" // as broker setting
#define LED_PIN 2 
PubSubClient client(m5stack);

void callback(char* topic, byte* payload, unsigned int length) {
 Serial.print("Message arrived ["); 
 Serial.print(topic); 
 Serial.print("] "); 
 String msg = ""; 
 int i=0; 
 while (i<length) 
  msg += (char)payload[i++]; 
 if (msg == "GET") { 
  client.publish("/nodeMCU/LED", (digitalRead(LED_PIN) ? "LEDON" : "LEDOFF")); 
  Serial.println("Send !"); 
  return; 
 } 
 digitalWrite(LED_PIN, (msg == "LEDON" ? HIGH : LOW)); 
 Serial.println(msg);
}

void setup() {
    // put your setup code here, to run once:
    // put your setup code here, to run once:
    Serial.begin(115200);
    WiFi.begin(ssid[0], pwd[0]);
 
    while (WiFi.status() != WL_CONNECTED) {
 
      delay(1000);
      Serial.println("Connecting..");
 
    }
    Serial.println("Connected");
    Serial.print("Ip address ");
    Serial.println(WiFi.localIP());
    Serial.println("Mac address " + WiFi.macAddress());
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("RSSI:");
    Serial.println(rssi);
    login.setWifiClient(m5stack);
    
    // Start Ethernet on Arduino
    // Print Update Response to Serial Monitor
    if (m5stack.available())
    {
      char c = m5stack.read();
      Serial.print(c);
    }

    //bool IsNet = false;
    bool IsNet = Ping.ping("www.google.com");
    delay(1000);
    
    while(!IsNet){
      Serial.println("logging to kmutnb net");
      IsNet = login.getKmutnbInternet();
      delay(1000);
    }
    
    Serial.println("logged to kmutnb net");
    Serial.println("m5stack is online");
    
    // mqtt setting 
    client.setServer(mqtt_server, mqtt_port);  //Sets the server details.  
    client.setCallback(callback); //Sets the message callback function.
    
}

void loop() {

}
