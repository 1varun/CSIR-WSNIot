#include <SPI.h>                                 //Library for SPI Communication
#include <WiFi.h>                                //Library for WiFi Shield
#include <WiFiUdp.h>                             //Library to recieve UNIX TimeStamp
#include <SD.h>                                  //Library to access SD Card

// size of buffer used to capture HTTP requests
#define REQ_BUF_SZ   20                          //Total number of characters recieve from GET  

File webFile;                                    //Variable to read from SD Card 
File myFile;;                                    //Variable to writ to SD Card

char ssid[] = "DLINK";
char pass[] = "mmtd@103";

int status = WL_IDLE_STATUS;
WiFiServer server(80);

unsigned long epoch = 0;
unsigned long previousMillis = 0;        //Will store last time data was updated
int allow = 0;                           //Tells if new data is recieved

unsigned int localPort = 2390;           // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28);    // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;          // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];     //buffer to hold incoming and outgoing packets
                                         
WiFiUDP Udp;
int h=0;                                 // Stores hours
int m=0;                                 // Stores minutes
int s=0;                                 // Stores seconds

int change = 0;                             
int allowswitch = 0;
float val = 0;
float O2 = 0;
float CO2 = 0;
float CO = 0;
float H2 = 0;
float H2S = 0;
float CH4 = 0;
float temp = 0;
float hum = 0;


char HTTP_req[REQ_BUF_SZ] = {0};        // buffered HTTP request
char req_index = 0;                     // index for HTTP_req buffer

void setup()
{    
  Serial.begin(9600);                   // for debugging
  Serial1.begin(9600);                  // for data monitoring

  while (!Serial) {
    ;
  }
    
                                        // initialize SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

                                        // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);                       // don't continue if no shield present
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.1.0") {
    Serial.println("Please upgrade the firmware");
  }

                                        // Connecting to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);               // print the network name (SSID);

    status = WiFi.begin(ssid, pass);    // Connect to WPA/WPA2 network.
    delay(10000);                       // wait 10 seconds for connection:
  }
  server.begin();                       // start the web server on port 80
  printWifiStatus();                    // you're connected now, so print out the status

  Serial.println("\nStarting connection to NTP server...");
  Udp.begin(localPort);

  sendNTPpacket(timeServer);            // send an NTP packet to a time server
  delay(2000);                          // wait to see if a reply is available
  Serial.println(Udp.parsePacket());
  if (Udp.parsePacket()) {
    Serial.println("packet received");  // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = ");
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    epoch = secsSince1900 - seventyYears + 19800UL;
    // print Unix time:
    Serial.println(epoch);
  }
  
  SD.remove("currdl.txt");
  Serial.println("Saving Title bar of CurrentDataLog");
  myFile = SD.open("currdl.txt", FILE_WRITE);
  myFile.println("LogTime  |  Oxy  | CO2 | CO  |  H2 | H2S | CH4 | Temp |  Hum |");
  myFile.println("**************************************************************");
  myFile.close();
  //Title bar added to logging file
  Serial.println("LogTime  |  Oxy  | CO2 | CO  |  H2 | H2S | CH4 | Temp |  Hum |");
  Serial.println("**************************************************************");
  //Title bar added to Serial output.

}

void loop()
{
  String dataOut = "";                      // Saves the data recieved from XBee
  String dataSave = "";                     // Saves the data and time
  String dataTime = "";                     // Saves only time 
  unsigned long currentMillis = millis();   // Gives the no milli second elapsed 

  while(Serial1.available())                // Reading from XBee
  {   
    dataOut = Serial1.readString();
    allow=1;
  }

  if(allow == 1){                            // Enters loop if data is sent by XBee
    epoch = epoch + ((currentMillis-previousMillis)/1000UL);
    previousMillis = currentMillis;

    h = ((epoch  % 86400L) / 3600);          // Calculating time
    if( h < 9){
      dataTime = dataTime + "0" + String(h);
    }
    else{
      dataTime = dataTime + String(h);
    }
    
    m = ((epoch  % 3600) / 60);
    if( m < 9){
      dataTime = dataTime + ":" + "0" + String(m);
    }
    else{
      dataTime = dataTime + ":" + String(m);
    }
    
    s = (epoch % 60);
    if( s < 9){
      dataTime = dataTime + ":" + "0" + String(s);
    }
    else{
      dataTime = dataTime + ":" + String(s);
    }

    dataTime = dataTime + " | ";
    

    int len = dataOut.length();              // Parsing data from string 
    int i = 0;
    String inString = "";
    char c = dataOut.charAt(i);
    while(c != '@'){                         //'@' signifies end of line
      if(c==','){                            //',' signifies end of a particular value
        val = inString.toFloat();            // val stores the value a variable temporarily
        inString = "";
        change = change + 1;
        allowswitch = 1;
      }
    
      if(allowswitch == 1){
        switch(change) {
          case 1:                            // Oxygen value retrieved
          O2 = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 2:                            // Carbin Dioxide value retrieved
          CO2 = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 3:                            // Carbon Monoxide value retrieved
          CO = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 4:                            // Hydrogen value retrieved
          H2 = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 5:                            // Hydrogen Sulphide value retrieved
          H2S = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 6:                            // Methane value retrieved
          CH4 = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 7:                            // Temperature value retrieved
          temp = val;
          dataSave = dataSave + String(val) + " |";
          break;

          case 8:                            // Humidity value retrieved
          hum = val;
          dataSave = dataSave + String(val) + " |";
          break;
        }
        allowswitch = 0;
      }
      if(c!=','){
        inString = inString + c;             // Stores new value after ','
      }
      c = dataOut.charAt(++i);               // new character from string is stored
    }
    change = 0;

    // open the file for reading:
    myFile = SD.open("currdl.txt", FILE_WRITE);
    // if the file opened okay, write to it:
    if (myFile) {                            // Writing the time and data to file
      myFile.print(dataTime); 
      myFile.println(dataSave);       
      myFile.close();                        //close the file
      Serial.print(dataTime);                // Writing the time and data to serial
      Serial.println(dataSave);
    }
    allow =0;
  }
  
  WiFiClient client = server.available();    // listen for incoming clients

  if (client) {                              // got client?
    //Serial.println("new client");
    String currentLine = "";
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {              // is client data available to read?
        char c = client.read();              // read 1 byte (character) from client
       
        // buffer first part of HTTP request in HTTP_req array
        // last element is set as null terminate string
        if (req_index < (REQ_BUF_SZ - 1)) {
          HTTP_req[req_index] = c;
          req_index++;
        }
        // print HTTP request character to serial monitor
        //Serial.write(c);
        // Client request ended if line of request ends with \n
        if (c == '\n'){
          if (currentLine.length() == 0) {
            // open requested web page file
            if (StrContains(HTTP_req, "GET / ")) {   //Sending the HTML file
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: text/html");
              client.println("Connnection: close");
              client.println("Refresh: 15");  // refresh the page automatically every 15 sec
              client.println();
  
              client.println("<!doctype html>");
              client.println("<html>");
              client.println("<style>");
          
              client.println("body {background-color: #66ffa6;}");
              client.print("h1{color:#ff3d00;font-weight:bold;font-size:2em;");
              client.print("font-family:Ariel, sans-serif;");
              client.println("border-left:6px solid #ff3d00;background-color:#b2fef7;}");
              client.print("h2{color:#263238;font-weight:bold;font-size:1.1em;");
              client.println("font-family:Ariel, sans-serif;}");
              client.print("h3{color:#263238;font-weight:bold;font-size:1.5em;");
              client.println("font-family:Ariel, sans-serif;}");
              client.print("p{color:#263238;font-weight:bold;font-size:1em;");
              client.print("font-family:Calibri, sans-serif; border: 2px solid red;");
              client.println("border-radius:5px; background-color:#e4e65e; padding:15px;}");
              client.print("p2{color:#263238;font-weight:bold;font-size:0.7em;");
              client.println("font-family:Ariel, sans-serif;}");
         
              client.println("</style>");
              client.println("<head>\n<meta charset='UTF-8'>");
              client.println("<title>CSIR Sensor Reading</title>");
              client.println("</head><body>");
              client.println("<h1>. ARDUINO's DATA .</h1>");
              client.println("<pre><h2> Reading from Environment Sensors:</h2>");
              client.print("<p>");
              
              client.print("<> Temp Reading is: ");
              client.print(temp);
              client.print("*C");
              client.println("<br>");

              client.print("<> Humidity Reading is: ");
              client.print(hum);
              client.print("%");
              client.println("<br>");

              client.print("<> O2 Reading is: ");
              client.print(O2);
              client.print("%");
              client.println("<br>");

              client.print("<> CO2 Reading is: ");
              client.print(CO2);
              client.print("%");
              client.println("<br>");;

              client.print("<> CO Reading is: ");
              client.print(CO);
              client.print("ppm");
              client.println("<br>");

              client.print("<> H2 Reading is: ");
              client.print(H2);
              client.print("ppm");
              client.println("<br>");

              client.print("<> H2S Reading is: ");
              client.print(H2S);
              client.print("ppm");
              client.println("<br>");

              client.print("<> CH4 Reading is: ");
              client.print(CH4);
              client.print("%");
              client.println("<br>");

              client.print("Click <a href=\"/CurrentDL.txt\">here</a> to get Current Data Log.<br>");
                            
              client.println("</p>");
              client.println("");
              client.print("<p2>***The page will be automatically refreshed in 20 secs.***</p2>");
              client.print("<h3>Thank You</h3>");
              client.print("</pre>");
              client.print("</body>");
              client.print("</html>");
            }
            else if (StrContains(HTTP_req, "GET /CurrentDL.txt")) {
              client.println("HTTP/1.1 200 OK");           // Sending the data from log file
              client.println("Content-Type: text/plain");  // Tells browser that content is plain text
              client.println("Connnection: close");
              client.println();
              webFile = SD.open("currdl.txt");        // open web page file
            }
            if (webFile) {
              byte clientBuf[64];
              int clientCount = 0;
         
              while(webFile.available()) {            // Reading from the file
                clientBuf[clientCount] = webFile.read();
                clientCount++;
        
               if(clientCount > 63)               
                {
                  // Serial.println("Packet");
                  client.write(clientBuf,64);
                  clientCount = 0;
                }
              }
              //final <64 byte cleanup packet
              if(clientCount > 0){
                client.write(clientBuf,clientCount);
              }
              webFile.close();
            }
            // reset buffer index and buffer elements to 0
            req_index = 0;
            StrClear(HTTP_req, REQ_BUF_SZ);
            break;
          } else {
              currentLineIsBlank = true;
              currentLine = "";
            }
        } 
        else if (c != '\r') {              // Storing the Client request 
          // a text character was received from client
          currentLineIsBlank = false;
          currentLine += c;
        }
      }                                    // end if (client.available())
    }                                      // end while (client.connected())
    delay(500);                            // give the web browser time to receive the data
    client.stop();                         // close the connection
    //Serial.println("client disonnected");
  }                                        // end if (client)
}

// sets every element of str to 0 (clears array)
void StrClear(char *str, char length)
{
  for (int i = 0; i < length; i++) {
      str[i] = 0;
  }
}

// searches for the string sfind in the string str
// returns 1 if string found
// returns 0 if string not found
char StrContains(char *str, char *sfind)
{
  char found = 0;
  char index = 0;
  char len;

  len = strlen(str);
    
  if (strlen(sfind) > len) {
    return 0;
  }
  while (index < len) {
    if (str[index] == sfind[found]) {
      found++;
      if (strlen(sfind) == found) {
        return 1;
      }
    }
    else {
      found = 0;
    }
    index++;
  }

  return 0;
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
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
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
