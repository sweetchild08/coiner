/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-static-fixed-ip-address-arduino/
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
 unsigned int addr = 0;

// Replace with your network credentials
const char* ssid     = "money-changer";
const char* password = "c0in1234";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)S
const long timeoutTime = 2000;

// Set your Static IP address
IPAddress local_IP(192, 168, 8, 20);
// Set your Gateway IP address
IPAddress gateway(192, 168, 8, 1);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional


char * host="www.itexmo.com";
char * url="/php_api/api.php";

char targetnumber[]="09760730306";
String APIcode="ST-SIRJE337008_XN49S";
String APIpass="keqj6h$(d%";

void sendsms(String a="sdas")
{
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  client.setInsecure();

//  Serial.printf("Using fingerprint '%s'\n", fingerprint);
//  client.setFingerprint(fingerprint);

  if (!client.connect(host, 443)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/php_api/api.php";
  Serial.print("requesting URL: ");
  Serial.println(url);
  
    String msg="1="+String(targetnumber)+"&2="+a+"&3="+APIcode+"&passwd="+APIpass;
    String q=String("POST ") + url + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Content-Type: application/x-www-form-urlencoded\r\n" +
          "Content-Length: "+msg.length()+"\r\n" +
          "\r\n" + msg
           + "\r\n";
           Serial.println(q);
    client.print(q);
  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}
void readnumber()
{
  addr=0;
  for(int i=0; i<11; ++i){
    targetnumber[i] = EEPROM.read(addr + i); //read individual byte from ram at (addr + i) and copy it to settings.myPW[i]
   }
}
void setup() {
  Serial.begin(115200);
  EEPROM.begin(sizeof(targetnumber));
  // Configures static IP address
//  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
//    Serial.println("STA Failed to configure");
//  }
  
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  readnumber();
   Serial.println(targetnumber);
  if (!MDNS.begin("money-changer")) {             // Start the mDNS responder for esp8266.local
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
}

void sendmessage(String used,String coinstat){
  Serial.print("used");
  Serial.println(used);
  Serial.print("constats");
  Serial.println(coinstat);
  String message="";
   message="Transaction!%0A";
   message+="%0A";
   message+="Client transaction recorded%0A";
   message+="Transacted Coins: "+used+"PHP%0A";
   message+="Remaining Coins: "+coinstat+"PHP%0A";
   message+="%0A";
   message+="%0A";
   message+="Coin-changer 2021\r\n";
   sendsms(message);
   if(coinstat.toInt()<=20) //alert
   {
     message="Coins alert!%0A";
     message+="%0A";
     message+="you're running out of coins%0A";
     message+="Coins: "+coinstat+"PHP%0A";
     message+="%0A";
     message+="%0A";
     message+="Coin-changer 2021%0A";
     sendsms(message);
   }
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients
   if(targetnumber[1]==0)
   {
    Serial.println("number not found assigning deafult");
    char number[12]= "09760730306";
    addr=0;
    for(int i=0; i<11; ++i){
        EEPROM.write(addr + i, number[i]);
    }
    
    EEPROM.commit();
   }
   delay(1000);
  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            if(header.indexOf("GET / HTTP/1.1")!=-1)
            {
              client.println("HTTP/1.1 302 Redirect");
              client.println("Location: /setphone");
              client.println();
            }
            else
            {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              bool issetphone=header.indexOf("setphone")!=-1;
              if(issetphone)
              {
                int setting=header.indexOf("phone=");
                if(setting!=-1)
                {
                  String ph=header.substring(setting+6);
                  char  number[12];
                  ph.toCharArray(number,12);
                  for(int i=0; i<11; ++i){
                      EEPROM.write(addr + i, number[i]);
                  }
                  EEPROM.commit();
                  readnumber();
                  client.println("<!DOCTYPE html><html>");
                  client.println("<head><meta http-equiv = \"refresh\" content = \"3; url = /setphone\" />");
                  // Web Page Heading
                  client.println("<body><center><h1>Coins Changer SMS Setting</h1>");
                  client.println("<h3 style=\"color:gray\">Setting phone to <span style=\"font-weight:bold\">"+String(number)+"</span></h3>");
                  client.println("</center></body></html>");
                }
                else{
                  // Display the HTML web page
                  client.println("<!DOCTYPE html><html>");
                  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                  client.println("<link rel=\"icon\" href=\"data:,\">");
                  // CSS to style the on/off buttons 
                  // Feel free to change the background-color and font-size attributes to fit your preferences
                  client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                  client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
                  client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                  client.println(".button2 {background-color: #77878A;}</style></head>");
                  
                  // Web Page Heading
                  client.println("<body><h1>Coins Changer SMS Setting</h1>");
                  
                  client.println("<center><form>");
                  client.println("<h1>Set Phone Number</h1>");
                  client.println("<h3>Current: <span style=\"font-weight:bold\">"+String(targetnumber)+"</span></h3>");
                  client.println("<label for=\"phone\">Set Phone Number</label>");
                  client.println("<input id=\"phone\" type=\"text\" name=\"phone\">");
                  client.println("<button type=\"submit\">Set</button>");
                  client.println("</form></center>");
                  client.println("</body></html>");
                }
              }
              else
              {
                int pos=header.indexOf("GET /")+5;
                int pos2=header.indexOf("/",pos+1);
     
                String trans=header.substring(pos,header.indexOf("/",pos+1));
                String coinstat=header.substring(pos2+1,header.indexOf(" ",pos2));
                Serial.println("sending values");
                sendmessage(coinstat,trans);
                client.println("Sending Message...");
              }
            }
            
     
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  MDNS.update();
}
