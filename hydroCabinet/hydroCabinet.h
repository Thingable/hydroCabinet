#ifndef HYDROCABINET_H
#define HYDROCABINET_H

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

class hydroCabinet{
  public:
    // Public Functions
    void init(HardwareSerial& Serialptr);
    void getTime(int& hour, int& minute);
    void clientHandler();
    void lights(HardwareSerial& Serialptr);
    
    // Public Server Attributes
    ESP8266WebServer server{80};
    WiFiClient client;
    WiFiUDP ntpUDP;
    NTPClient timeClient{ntpUDP, 3600*19};
    
  private:
    // Server Routes
    void handleRoot();
    void handleUpdate();
    void handleNotFound();
    
    // Private Attributes
    int startHour;
    int startMin;
    int onHour;
    int onMin;
    int offHour;
    int offMin;
    bool light = 0;

    //Pin Delclarations
    const int LIGHT_CONTROL = 12;
    
    const char* ssid     = "FireFi";
    const char* password = "yd995bqb84";
    const unsigned long HTTP_TIMEOUT = 10000;  // max respone time from server
    const size_t MAX_CONTENT_SIZE = 512;       // max size of the HTTP response
};

// *** Function Definitions *** //


/*
 * 
 */
void hydroCabinet::init(HardwareSerial& Serialptr){
  pinMode(LIGHT_CONTROL, OUTPUT);
  
  // Connect to WiFi network
  Serialptr.print("\n\r \n\rWorking to connect");
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serialptr.print(".");
  }

  Serialptr.print("Connected to ");
  Serialptr.println(ssid);
  Serialptr.print("IP address: ");
  Serialptr.println(WiFi.localIP());

  // Setup server functions
  //server.on("/", handleRoot);
  server.on("/", std::bind(&hydroCabinet::handleRoot, this));
  server.on("/update", std::bind(&hydroCabinet::handleUpdate, this));
  server.onNotFound(std::bind(&hydroCabinet::handleNotFound, this));

  server.begin();
  Serialptr.println("HTTP server started");
  Serialptr.println();

  Serialptr.print("Initializing clock (please wait 10 seconds)");
  timeClient.begin();
  for(int i=0; i<10; i++){
    Serial.print(".");
    delay(1000);
  }
  Serialptr.println();
  
  Serialptr.println("Initilizing hydroCabinet Object");
  getTime(startHour, startMin);

  Serialptr.println("Ready to grow!");
}

/*
 * 
 */
void hydroCabinet::clientHandler(){
  server.handleClient();
}

/*
 * 
 */
void hydroCabinet::handleRoot() {
  int currentHour;
  int currentMin;
  getTime(currentHour, currentMin);
 
 
  String message = ""
"<html>"
  "<head>"
    "</head>"
    "<body>"
        "<h1>hydroCabinet - v0.05</h1>"
        "<form action='update'>"
          "Start Time:"
          "<select name='onHour'>"
              "<option value='00'>00</option>"
              "<option value='1'>1</option>"
              "<option value='2'>2</option>"
              "<option value='3'>3</option>"
              "<option value='4'>4</option>"
              "<option value='5'>5</option>"
              "<option value='6'>6</option>"
              "<option value='7'>7</option>"
              "<option value='8'>8</option>"
              "<option value='9'>9</option>"
              "<option value='10'>10</option>"
              "<option value='11'>11</option>"
              "<option value='12'>12</option>"
              "<option value='13'>13</option>"
              "<option value='14'>14</option>"
              "<option value='15'>15</option>"
              "<option value='16'>16</option>"
              "<option value='17'>17</option>"
              "<option value='18'>18</option>"
              "<option value='19'>19</option>"
              "<option value='20'>20</option>"
              "<option value='21'>21</option>"
              "<option value='22'>22</option>"
              "<option value='23'>23</option>"
          "</select>"
          "<select name='onMin'>"
              "<option value='5'>5</option>"
              "<option value='10'>10</option>"
              "<option value='15'>15</option>"
              "<option value='20'>20</option>"
              "<option value='25'>25</option>"
              "<option value='30'>30</option>"
              "<option value='35'>35</option>"
              "<option value='40'>40</option>"
              "<option value='45'>45</option>"
              "<option value='50'>50</option>"
              "<option value='55'>55</option>"
          "</select>"
          "</br>"
          "End Time:"
          "<select name='offHour'>"
              "<option value='00'>00</option>"
              "<option value='1'>1</option>"
              "<option value='2'>2</option>"
              "<option value='3'>3</option>"
              "<option value='4'>4</option>"
              "<option value='5'>5</option>"
              "<option value='6'>6</option>"
              "<option value='7'>7</option>"
              "<option value='8'>8</option>"
              "<option value='9'>9</option>"
              "<option value='10'>10</option>"
              "<option value='11'>11</option>"
              "<option value='12'>12</option>"
              "<option value='13'>13</option>"
              "<option value='14'>14</option>"
              "<option value='15'>15</option>"
              "<option value='16'>16</option>"
              "<option value='17'>17</option>"
              "<option value='18'>18</option>"
              "<option value='19'>19</option>"
              "<option value='20'>20</option>"
              "<option value='21'>21</option>"
              "<option value='22'>22</option>"
              "<option value='23'>23</option>"
          "</select>"
          "<select name='offMin'>"
              "<option value='5'>5</option>"
              "<option value='10'>10</option>"
              "<option value='15'>15</option>"
              "<option value='20'>20</option>"
              "<option value='25'>25</option>"
              "<option value='30'>30</option>"
              "<option value='35'>35</option>"
              "<option value='40'>40</option>"
              "<option value='45'>45</option>"
              "<option value='50'>50</option>"
              "<option value='55'>55</option>"
          "</select>"
          "<input type='submit'>"
        "</form>"
    "</body>"
"</html>";
  server.send(200, "text/html", message);
}

/*
 * 
 */
void hydroCabinet::handleUpdate() {
  String onTime;
  String offTime;
  if (server.args() > 0 ){
    Serial.println(server.args());
    for ( uint8_t i = 0; i < server.args(); i++ ){
      Serial.println(server.argName(i));
      if(server.argName(i) == "onHour"){
        onHour = server.arg(i).toInt();
      }else if(server.argName(i) == "onMin"){
        onMin = server.arg(i).toInt();
      }else if(server.argName(i) == "offHour"){
        offHour = server.arg(i).toInt();
      }else if(server.argName(i) == "offMin"){
        offMin = server.arg(i).toInt();
      }  
    }
  } 
  String message = "On: " + String(onHour) + ":" + String(onMin);
  message += "</br>Off: " + String(offHour) + ":" + String(offMin);
  server.send(200, "text/html", message);
}

/*
 * 
 */
void hydroCabinet::handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

/*
 * 
 */
void hydroCabinet::getTime(int& hour, int& minute){
  timeClient.update();
  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
}

/*
 * 
 */
void hydroCabinet::lights(HardwareSerial& Serialptr){
  int currentHour;
  int currentMin;
  getTime(currentHour, currentMin);

  String message = " On: " + String(onHour) + ":" + String(onMin);
          message += " Off: " + String(offHour) + ":" + String(offMin);
          message += " Current: " + String(currentHour) + ":" + String(currentMin);
  Serialptr.println(message);

  if((currentHour > onHour || (currentHour == onHour && currentMin >= onMin)) && (currentHour < offHour || (currentHour == offHour && currentMin <= offMin))){
    // Lights on
    light = 1;
    Serialptr.println("Lights on");
  }else{
    // Lights off
    light = 0;
    Serialptr.println("Lights off");
  }

  digitalWrite(LIGHT_CONTROL, light);
}

#endif 

