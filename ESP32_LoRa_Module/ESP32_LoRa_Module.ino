#include "heltec.h"
#include "images.h"
#include "WiFi.h"

// Network SSID
char ssid[] = "Agrilog";
// Network Pass
char pass[] = "12345678";

char outputString[80];

int wifi_status = WL_IDLE_STATUS;
// IP-address or domain for target destination.
char server[] = "192.168.1.2";

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
    sprintf(outputString, "Attempts to connect: %d", counter++);
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, outputString);
    Heltec.display->display();
    wifi_status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  // Check if connection failed.
  if (wifi_status != WL_CONNECTED)
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Could not connect to WiFi");
    Heltec.display->display();

  } else {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Connected to Wifi");
    Heltec.display->display();
  }
  delay(1000);
  LoRa.receive();
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Waiting for incoming data...");
  Heltec.display->display();
  
}

void loop()
{
  packetSize = LoRa.parsePacket();
  if (packetSize == 15) {
    Heltec.display->clear();
    rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
    Heltec.display->drawString(0, 0, rssi);
    Heltec.display->drawString(0, 15, "Received packet:");

    byte buffer[packetSize];
    packet.getBytes(buffer, packetSize);
      
    while (LoRa.available()) {
      for (int i = 0; i < packetSize; i++) {
        packet += (char) LoRa.read();
      }
      for (int i = 0; i < 16; i++) {
          Heltec.display->drawString(i * 6, 26, String(packet[i], HEX));
          i++;
        }
      Serial.println("END OF PACKET");
      Heltec.display->display();
    }
    while (client.connect(server, 8085))
    {
      // Data
      String queryString = String("byte=") + String(packet[0], HEX) + "." +
                                             String(packet[1], HEX) + "." +
                                             String(packet[2], HEX) + "." +
                                             String(packet[3], HEX) + "." +
                                             String(packet[4], HEX) + "." +
                                             String(packet[5], HEX) + "." +
                                             String(packet[6], HEX) + "." +
                                             String(packet[7], HEX) + "." +
                                             String(packet[8], HEX) + "." +
                                             String(packet[9], HEX) + "." +
                                             String(packet[10], HEX) + "." +
                                             String(packet[11], HEX) + "." +
                                             String(packet[12], HEX) + "." +
                                             String(packet[13], HEX) + "." +
                                             String(packet[14], HEX) +
                           String("&device_uuid=") + String("27c6e815-b714-4ae8-93fd-53db35069a2a");

      // Send to server through a HTTP POST request.
      client.println("POST /agrilog-server/insert-byte.php HTTP/1.1");
      client.println("Host: 192.168.1.2:8085");
      client.println("Connection: Keep-Alive");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(queryString.length()));
      client.println(); // end HTTP header
      client.println(queryString);
      for (int i = 0; i < packetSize + 1; i++) {
        Serial.println(buffer[i], HEX);
      }
      Serial.println("Packet sent.");   

      Heltec.display->drawString(0, 48, "Packet sent.");
      Heltec.display->display();

      delay(300000);
    }
  }
}
