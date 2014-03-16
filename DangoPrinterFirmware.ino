#include <SoftwareSerial.h>

#include <HTTPClient.h>

#include <SPI.h>
#include <HttpClient.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Metro.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//byte server[] = {192,168,1,5};
const char kHostname[] = "api.xks.im";
const char kPath[] = "/dangoprinter/message-raw";



//unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
//boolean lastConnected = false;                    // state of the connection last time through the main loop
//const unsigned long postingInterval = 5L*1000L;  // delay between updates, in millisecond

EthernetClient client;
HttpClient http(client);
SoftwareSerial ps(2, 3); // RX, TX
Metro sendingMetro = Metro(1000L);

// Number of milliseconds to wait without receiving any data before we give up
const int kNetworkTimeout = 30*1000;
// Number of milliseconds to wait if no data is available before trying again
const int kNetworkDelay = 1;

void setup() {
  // put your setup code here, to run once:
  ps.begin(9600);
  ps.write(0x1c);
  ps.write(0x40);
  ps.write(0x04);  
  ps.begin(38400);
  Ethernet.begin(mac);
  delay(1000);
  ps.print("DangoPrinter\n");
  ps.print(Ethernet.localIP());
  ps.print("\n\n\n");
  //while (true);
}

void loop() {
  if(sendingMetro.check()) {    
    http.get(kHostname, kPath);
    int responseCode = http.responseStatusCode();
    if (responseCode == 200){
      http.skipResponseHeaders();
        int bodyLen = http.contentLength();
        // Now we've got to the body, so we can print it out
        unsigned long timeoutStart = millis();
        char c;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ( (http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout) )
        {
            if (http.available())
            {
                c = http.read();
                // Print out this character
                ps.write(c);
                //ps.println(c,HEX);
                bodyLen--;
                // We read something, reset the timeout counter
                timeoutStart = millis();
            }
            else
            {
                // We haven't got any data, so let's pause to allow some to
                // arrive
                delay(kNetworkDelay);
            }
        }
      
    } else if (responseCode == 201){
      
    }
    http.stop();
    
  }

}
    
 
