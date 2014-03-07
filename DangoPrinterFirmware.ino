#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
char server[] = "192.168.1.5";

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
boolean lastConnected = false;                    // state of the connection last time through the main loop
const unsigned long postingInterval = 5L*1000L;  // delay between updates, in millisecond

EthernetClient client;
SoftwareSerial ps(2, 3); // RX, TX
void setup() {
  // put your setup code here, to run once:
  ps.begin(9600);
  Ethernet.begin(mac);
  delay(1000);
  //while (true);
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    ps.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected) {
    ps.println();
    ps.println("disconnecting.");
    client.stop();
  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    httpRequest();
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

void httpRequest() {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    ps.println("connecting...");
    // send the HTTP PUT request:
    client.println(F("GET /dangoprinter/message HTTP/1.1"));
    client.println(F("Host: localhost"));
    client.println(F("User-Agent: arduino-ethernet"));
    client.println(F("Connection: close"));
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    ps.println("connection failed");
    ps.println("disconnecting.");
    client.stop();
  }
}

