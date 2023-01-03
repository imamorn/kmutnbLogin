/**  
  Author: Amornphun Phunopas
  Department of Production and Robotics Engineering,
  Faculty of Engineering, 
  King Mongkut's University of Technology North Bangkok
  
  Description: The library is for logging to the internet in Kmutnb through FortiGate firewall.
  A user must have his own user and password for using this library.
  
  controller: m5stack 
  
**/

#include <Arduino.h>

#include <HTTPClient.h>
#include <ESP32Ping.h>
#include <WiFiClientSecure.h>

class kmutnbLogin{

  private:
    String username;
    String password;
    String session_id;
    WiFiClient wificlient;
    uint16_t httpsPort = 443;
    String url = "http://google.com"; // "https://captive.apple.com";
    HTTPClient http_first;  // get request
    HTTPClient http_second; // get request
    HTTPClient http_third;  // post request
    
  public:
    kmutnbLogin(String user, String pwd);
    void setWifiClient(WiFiClient client);
    WiFiClient getWifiClient();
    String splitURL(String msg);
    String getDataMagic(String msg);
    bool getKmutnbInternet();
};
