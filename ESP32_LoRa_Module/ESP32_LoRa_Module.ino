#include "heltec.h"
#include "images.h"
#include "WiFi.h"

// Network SSID
char ssid[] = "Agrilog";
// Network Pass
char pass[] = "12345678";         

char output_string[80];

int wifi_status = WL_IDLE_STATUS;
char server[] = "192.168.1.2";    // IP-address or domain for target destination.

#define BAND 433920000
#define PABOOST false
#define TXPOWER 14
#define SPREADING_FACTOR 7
#define BANDWIDTH 500000
#define CODING_RATE 5
#define PREAMBLE_LENGTH 6
#define SYNC_WORD 0x34

String rssi = "RSSI --";
String packSize = "--";
String packet ;

int packetSize;

void logo() {
  Heltec.display->clear();
  Heltec.display->drawXbm(0, 5, logo_width, logo_height, logo_bits);
  Heltec.display->display();
}

void LoRaData() {
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  //Heltec.display->drawStringMaxWidth(0 , 26 , 128, packet);

  if (packetSize == 15)
  {
    Heltec.display->drawString(0 , 15 , "Received " + packSize + " bytes");
    Heltec.display->drawString(0 , 26 , "Weather:");
    //Heltec.display->drawString(0 , 37 , "Weather:");
    for ( int i = 0; i < 15; i++)
    {
      Heltec.display->drawString(i * 6, 37, String(packet[i], HEX));
      i++;
    }

    int device = packet[1] >> 4;
    uint16_t temp = lowByte(packet[8]) << 8 + packet[9];
    Heltec.display->drawString(0, 48, "." + String(device, HEX) + "." + String(packet[3], DEC) + "." + String(temp, DEC));
  } else
  {
    Heltec.display->drawString(0 , 26 , "Other packet...:");
  }
  Heltec.display->drawString(0, 0, rssi);
  Heltec.display->display();
}

void cbk(int packetSize) {
  packet = "";
  packSize = String(packetSize, DEC);
  for (int i = 0; i < packetSize; i++) {
    packet += (char) LoRa.read();
  }
  rssi = "RSSI " + String(LoRa.packetRssi(), DEC) ;
  LoRaData();
}

// Initialize the client library
WiFiClient client;

void setup()
{
  // Initialize module
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(1500);

  LoRa.setFrequency(433920000);
  //LoRa.setSyncWord(0xF3);
  LoRa.setSyncWord(0x34);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(5);
  LoRa.setSignalBandwidth(500000);
  LoRa.setPreambleLength(6);
  LoRa.enableCrc();

  // Attempt to connect to WiFi.
  while (wifi_status != WL_CONNECTED)
  {
    static int counter = 0;

          sprintf(output_string, "Attempts to connect: %d", counter++);
          Heltec.display->clear();
          Heltec.display->drawString(0, 0, output_string);
          Heltec.display->display();
          wifi_status = WiFi.begin(ssid, pass);

    // Wait 5 sec
    delay(5000);
  }
  
  // Check if connection failed.
  if (wifi_status != WL_CONNECTED) 
  {
        
         Heltec.display->clear();
         Heltec.display->drawString(0, 0, "Could not connect to WiFi");
         Heltec.display->display();
 
  }
  else 
  {
         Heltec.display->clear();
         Heltec.display->drawString(0, 0, "Connected to Wifi");
         Heltec.display->display();
         delay(500);
  }
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Waiting for incoming data...");
  Heltec.display->display();
  LoRa.receive();
}

void loop()
{
  packetSize = LoRa.parsePacket();

  if (packetSize == 15) {
    cbk(packetSize);
    if (client.connect(server, 8085)) 
    {
      // Data
      String queryString = String("byte=") + String(packet[0]) + 
                                             String(packet[1]) + 
                                             String(packet[2]) + 
                                             String(packet[3]) + 
                                             String(packet[4]) + 
                                             String(packet[5]) + 
                                             String(packet[6]) + 
                                             String(packet[7]) + 
                                             String(packet[8]) + 
                                             String(packet[9]) + 
                                             String(packet[10]) + 
                                             String(packet[11]) + 
                                             String(packet[12]) + 
                                             String(packet[13]) + 
                                             String(packet[14]);

      // Send to server through a HTTP POST request.
      client.println("POST /agrilog-server/insert-byte.php HTTP/1.1");
      client.println("Host: 192.168.1.2:8085");
      client.println("Connection: Keep-Alive");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(queryString.length()));
      client.println(); // end HTTP header
      client.println(queryString);
    } else {
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Couldn't connect to server.");
      Heltec.display->display();
    }
  }  
}
