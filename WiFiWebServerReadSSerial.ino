/*
  WiFi Web Server

 A simple web server that shows the value of the analog input pins.
 using a WiFi shield.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * Analog inputs attached to pins A0 through A5 (optional)

 created 13 July 2010
 by dlf (Metodo2 srl)
 modified 31 May 2012
 by Tom Igoe

 */

#include <SPI.h>
#include <WiFi.h>


#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9); // RX, TX



int uid[7]={255,255,255,255,255,255,255};
char ssid[] = "iPhone de Alejandro";      // your network SSID (name)
char pass[] = "12345678";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(90);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  mySerial.begin(4800);
  
  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  server.begin();
  // you're connected now, so print out the status:
  printWifiStatus();

}


void loop() {
  //Listen for mySerial
   
   /*if (mySerial.available()) {
    for(int i=0; i<7; i++)
    {
      uid[i] = mySerial.read();
    }
    
  }
  */
  // listen for incoming clients
  WiFiClient client = server.available();
  if (client) {

    
    Serial.println("new client");
    


    
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");


client.println("<head>");
  client.println("<title>Lector RFID</title>");
 
  client.println("<style>body{background-color: rgb(25, 25, 112);font-family: Verdana, Arial;}");
  client.println("table{background-color: white;}");
  client.println("td, th {border-left:solid black 1px;border-top:solid black 1px;}");
  client.println("th {background-color: rgb(32, 178, 170);border-top: none;}");
  client.println("td:first-child, th:first-child {border-left: none;}</style>");
 
  client.println("</style>");
client.println("</head>");
client.println("<body onload=\"myF();\">");
  client.println("<h1 align=\"center\">Proyecto de Robotica</h1>");
  client.println("<h2 align=\"center\" >Lector de RFID</h2>");
  client.println("<table align=\"center\" width=\"500\"><thead><tr><th>UUID</th>");
  client.println("<th>Fecha</th></tr></thead><tr><td align=\"center\">");
      if(mySerial.available()==0)
      {
        client.println("Tarjeta no leida");
      }
      while(mySerial.available()>0)
      {
         client.println(mySerial.read());
      }
      client.println("</td><td align=\"center\" id=\"fecha\">"); 
      client.println("</td></tr></table></br>");

      client.println("<script>function myF(){var d = new Date();var n = d.toLocaleString();");
      client.println("document.getElementById(\"fecha\").innerHTML = n;}</script></body></html>");


          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);

    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

