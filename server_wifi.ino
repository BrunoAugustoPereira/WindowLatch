/*
 DHCP Chat  Server

 A simple server that distributes any incoming messages to all
 connected clients.  To use, telnet to your device's IP address and type.
 You can see the client's input in the serial monitor as well.
 Using an Arduino Wiznet Ethernet shield.

 THis version attempts to get an IP address using DHCP

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 21 May 2011
 modified 9 Apr 2012
 by Tom Igoe
 modified 02 Sept 2015
 by Arturo Guadalupi
 Based on ChatServer example by David A. Mellis

 */
#include <AESLib.h>
#include <SPI.h>
#include <WiFi.h>
#include <Crc16.h>
#include <stdio.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
char ssid[] = "yourNetwork"; 
char pass[] = "secretPassword";

byte mac[] = {
  0x90, 0xA2, 0xDA, 0x0F, 0x46, 0x77  
};

IPAddress ip(192, 168, 1, 177);
Crc16 crc; 
// telnet defaults to port 23
WiFiServer server(23);
boolean gotAMessage = false; // whether or not you got a message from the client yet
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // start the Ethernet connection:
  /*Serial.println("Trying to get an IP address using DHCP");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
  }*/
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
  // print your local IP address:
  Serial.print("My IP address: ");
  ip = Ethernet.localIP();
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(ip[thisByte], DEC);
    Serial.print(".");
  }
  Serial.println();
  // start listening for clients
  server.begin();
  

}

void loop() {
  // wait for a new client:
  
  EthernetClient client = server.available();
  char data[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  // when the client sends the first byte, say hello:
  if (client) {
    if (!gotAMessage) {
      Serial.println("We have a new client");
      client.println("Hello, client!");
      gotAMessage = true;
    }

    // read the bytes incoming from the client:
   char thisChar;
    for(int i=0;i<18;i++){
      thisChar = client.read();
      Serial.print("lido: ");
      Serial.println(thisChar);
      data[i]=thisChar;
      if(i==17){
      aes128_dec_single(key, data);
     // data[16]
      Serial.print("desencript:");
      Serial.println(data);
      }
      }
       unsigned short verify = calcrc((char*)data, 18);   //USAR A MESMA FUNÇÃO, POIS DIVIDINDO PELO POLINOMIO 0x1021 -> CRC DEVERÁ SER 0. VETOR msg[] = MENSAGEM RECEBIDA.

      
     if(verify==0){  //SE CRC=0, LOGO NENHUM DADO FOI CORROMPIDO.
 
     Serial.println("\nNENHUM DADO FOI CORROMPIDO."); 
    }
    else{
    Serial.println("\nERRO: OS DADOS FORAM CORROMPIDOS."); 
    }

    // echo the bytes back to the client:
    server.write(thisChar);
    // echo the bytes to the server as well:
    Ethernet.maintain();
  }
}
int calcrc(char *ptr, int count)
{
    int  crc;
    char i;
    crc = 0;
    while (--count >= 0)
    {
        crc = crc ^ (int) *ptr++ << 8;
        i = 8;
        do
        {
            if (crc & 0x8000)
                crc = crc << 1 ^ 0x1021;
            else
                crc = crc << 1;
        } while(--i);
    }
    return (crc);
}
