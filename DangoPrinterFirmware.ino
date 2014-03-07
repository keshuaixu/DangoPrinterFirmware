#include <HTTPClient.h>

#include <SoftwareSerial.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Metro.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte server[] = {192,168,1,5};

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
boolean lastConnected = false;                    // state of the connection last time through the main loop
const unsigned long postingInterval = 5L*1000L;  // delay between updates, in millisecond

EthernetClient client;
SoftwareSerial ps(2, 3); // RX, TX
Metro sendingMetro = Metro(10000L);

char data[40];

void setup() {
  // put your setup code here, to run once:
  ps.begin(9600);
  Ethernet.begin(mac);
  delay(1000);
  //while (true);
}

void loop() {
  if(sendingMetro.check() && !client.connected() ) {
    HTTPClient client( "KESHUAI-PC", server );
    FILE* result = client.getURI("/dangoprinter/message");
    const int MAX_SIZE = 10;
    char buf[MAX_SIZE];
    if (!fgets(buf, MAX_SIZE, result)) {
      ps.println("error");
    } else {
      ps.write(buf);
    }    
    
    /*
////////
    int inByte = 0;
    while (inByte != EOF){      
      inByte = fgetc(result);
      ps.write(inByte);      
    }
////////
*/
    int returnCode = client.getLastReturnCode();
    
    if (result!=NULL) {
      client.closeStream(result);  // this is very important -- be sure to close the STREAM
    } 
    else {
      ps.println("failed to connect");
    }
    
    if (returnCode==200) {
      ps.println("data uploaded");
    } 
    else {
      ps.print("ERROR: Server returned ");
      ps.println(returnCode);
    }
  }
  
  
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}
    
 
