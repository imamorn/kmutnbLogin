/**  
  Author: Amornphun Phunopas
  Department of Production and Robotics Engineering,
  Faculty of Engineering, 
  King Mongkut's University of Technology North Bangkok
  
  Description: The library is for logging to the internet in Kmutnb through FortiGate firewall.
  A user must have his own user and password for using this library.
  
  controller: m5stack 
  
**/
 
#include "kmutnbLogin.h"

kmutnbLogin::kmutnbLogin(String user, String pwd){
  username = user;
  password = pwd;
}

void kmutnbLogin::setWifiClient(WiFiClient client){
  wificlient = client;
}

WiFiClient kmutnbLogin::getWifiClient(){
  return wificlient;
}

String kmutnbLogin::splitURL(String msg){
 int ind1 = msg.indexOf('"'); //finds location of first ,
 int ind2 = msg.indexOf('"', ind1+1 ); //finds location of second ,
 int ind3 = msg.indexOf('"', ind2+1 ); //finds location of third ,
 int ind4 = msg.indexOf('"', ind3+1 ); //finds location of fourth ,
 
 //String fuel = msg.substring(ind1+1, ind2); //captures second data String
 String fuel_ = msg.substring(ind3+1, ind4); //captures fourth data String
 
 Serial.println(msg);
 return fuel_;
}

String kmutnbLogin::getDataMagic(String msg){
  int ind1 = msg.indexOf('?');
  String fuel = msg.substring(ind1+1, msg.length());   //captures second data String
  return fuel;
}

bool kmutnbLogin::getKmutnbInternet(){
  http_first.begin(url); //Specify request destination
  int httpCode = http_first.GET(); //Send the request
  
  if (httpCode > 0) { //Check the returning code
    // First step, getting session_id ------------------
    Serial.println("step 1");
    String payload = http_first.getString();
    String response = splitURL(payload);
    String session_id = getDataMagic(response);
    String newLogin = "username=";
          newLogin += username;
          newLogin += "&password=";
          newLogin += password;
          newLogin += "&magic=";
          newLogin += session_id;
          newLogin += "&4Tredir=";
          newLogin += url;

    http_first.end();
    Serial.println(session_id);
    
    // Second step, Http get request ------------------------
    Serial.println("step 2");
    String serverName = "https://security.eng.kmutnb.ac.th:1003/fgtauth?"+session_id;   
    char serverNameBuf[serverName.length()];
    serverName.toCharArray(serverNameBuf, serverName.length());
    WiFiClientSecure client;
    client.setInsecure(); //the magic line, use with caution
    client.connect(serverNameBuf, httpsPort);
    
    http_second.begin(client, serverName);
    int httpGetResponseCode = http_second.GET();
    Serial.println(httpGetResponseCode);
    if (httpGetResponseCode > 0) {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpGetResponseCode);

      // file found at server
      if (httpGetResponseCode == HTTP_CODE_OK) {
        const String& payload = http_second.getString();
        //Serial.println("received payload:\n<<");
        //Serial.println(payload);
        //Serial.println(">>");
      }

    } else {
      //Serial.printf("[HTTP] GET... failed, error: %s\n", http_get_request.errorToString(httpGetResponseCode).c_str());
      return false;
    }

    http_second.end(); //Close connection
    
    // Third step, Http post request ------------------------
    Serial.println("step 3");
    //serverName = "https://security.eng.kmutnb.ac.th:1003/"; 
    http_third.begin(client, serverName);
    http_third.addHeader("Content-Type", "application/x-www-form-urlencoded");
    http_third.addHeader("Content-length", String(newLogin.length()));
    http_third.addHeader("Accept-Encoding", "gzip, deflate, br");
    http_third.addHeader("Connection","close"); // setting options "keep-alive" or "close"

    int httpResponseCode = http_third.POST(newLogin);
    if (httpResponseCode > 0) {
      // HTTP header has been send and Server response header has been handled
      //Serial.printf("[HTTP] POST... code: %d\n", httpResponseCode);

      // file found at server
      if (httpResponseCode == HTTP_CODE_OK) {
        const String& payload = http_third.getString();
        //Serial.println("received payload:\n<<");
        //Serial.println(payload);
        //Serial.println(">>");
      }

    } else {
      //Serial.printf("[HTTP] POST... failed, error: %s\n", http_second_request.errorToString(httpResponseCode).c_str());
      return false;
    }

    // Try to ping internet
    bool ret = Ping.ping("www.google.com", 10);
    if(ret)
      return true;
    else
      return false;
      
  } // End
  else{
    // http_first request
    Serial.printf("Error code: %d\n", httpCode);
  }
  return false;
}
