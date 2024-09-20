#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <OneWire.h>
#include <DallasTemperature.h>

const int oneWireBus = 4;     

OneWire oneWire(oneWireBus);

DallasTemperature sensors(&oneWire);

const char* ssid     = "Tab";
const char* password = "T77777777";

// Setting Static IP.
        IPAddress local_IP(192, 168, 1, 115);
        IPAddress gateway(192, 168, 1, 1);
        IPAddress subnet(255, 255, 255, 0); 
        IPAddress primaryDNS(8, 8, 8, 8); //opcional 
        IPAddress secondaryDNS(8, 8, 4, 4); //opcional 

WiFiServer server(80); // Port 80

#define relay  16    // LED12

String estado = "";
int wait30 = 30000; // time to reconnect when connection is lost.
float temperatureC = sensors.getTempCByIndex(0);
String setted_tem = "";

void setup() {
  Serial.begin(115200);

  pinMode(relay, OUTPUT);

// Setting Static IP.
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("Error in configuration.");
  }

// Connect WiFi net.
  Serial.println();
  Serial.print("Connecting with ");
  Serial.println(ssid);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected with WiFi.");
 
  // Start Web Server.
  server.begin();
  Serial.println("Web Server started.");
 
  // This is IP
  Serial.print("This is IP to connect to the WebServer: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());
  sensors.begin();
  
}
 
void loop() {
// If disconnected, try to reconnect every 30 seconds.
  if ((WiFi.status() != WL_CONNECTED) && (millis() > wait30)) {
    Serial.println("Trying to reconnect WiFi...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    wait30 = millis() + 30000;
  } 
  // Check if a client has connected..
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
   
  Serial.print("New client: ");
  Serial.println(client.remoteIP());
   
  // Espera hasta que el cliente envíe datos.
  // while(!client.available()){ delay(1); }

  // Read the information sent by the client.
  String req = client.readStringUntil('\r');
  Serial.println(req);
  req.replace("+", " ");          // Para que los espacios no salgan con +
  req.replace(" HTTP/1.1", "");   // Para quitar HTTP/1.1
  req.replace("GET /", ""); 
  char buf[req.length()+1];
  req.toCharArray(buf,req.length()+1);
  int num = atoi(req.c_str());
  if (num != 0){
    setted_tem = num ;
    Serial.println(setted_tem);
  }
  
  get_tem();
  // Make the client's request.
       if (req.indexOf("consulta") != -1){
          estado ="";
          get_tem();
          estado =sensors.getTempCByIndex(0);
                 if (setted_tem > estado)  {
                  Serial.println("heating");
                  Serial.println(setted_tem);
                  Serial.println(estado);
                  digitalWrite(relay, HIGH);
                  
                }else{
                  digitalWrite(relay, LOW);
                }
           }

  // Página WEB. ////////////////////////////
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println(estado);
  
  
  client.flush();
  client.stop();
  Serial.println("Client disconnected.");
}

void get_tem() {
  sensors.requestTemperatures(); 
}
