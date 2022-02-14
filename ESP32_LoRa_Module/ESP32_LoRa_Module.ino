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
  delay(500);

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
    Heltec.display->drawString(0, 0, "Couldn't connect to WiFi");
    Heltec.display->display();

  } else {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Connected to Wifi");
    Heltec.display->display();
    delay(1500);
    
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Waiting for incoming data...");
    Heltec.display->display();
    
    LoRa.receive();
  }
  
}

void loop()
{
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Loop start");
  Heltec.display->display();
  
  packetSize = LoRa.parsePacket();

  // Packet received
  if (packetSize) {
    if (packetSize == 15) {
      Heltec.display->clear();
      rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
      Heltec.display->drawString(0, 0, rssi);
      Heltec.display->drawString(0, 15, "Received packet:");

      // Display packet
      for (int i = 0; i < packetSize; i++) {
        packet += (char) LoRa.read();
      }
      for (int i = 0; i < 16; i++) {
        Heltec.display->drawString(i * 6, 26, String(packet[i], HEX));
        i++;
      }

      Heltec.display->display();

      // Connect to server
      if (client.connect(server, 8085))
      {
        // Data to input
        int rainfall = packet[6] + packet[7];
        int tempBits = packet[8] & 0x0F;
        int temperature = tempBits + packet[9];
        Serial.println(temperature);
        int light = packet[11] + packet[12];
        String queryString = String("device=") + String(packet[0], HEX) +
                             String("&station_name=") + String(packet[2], HEX) +
                             String("&average_wind_speed=") + String(packet[3], DEC) +
                             String("&gust_wind_speed=") + String(packet[4], DEC) +
                             String("&wind_direction=") + String(packet[5], DEC) +
                             String("&rainfall=") + String(rainfall, DEC) +
                             String("&temperature=") + String(packet[9], DEC) +
                             String("&humidity=") + String(packet[10], HEX) +
                             String("&light=") + String(light, DEC) +
                             String("&uvi=") + String(packet[13], DEC);

        // Send data to server through HTTP POST
        client.println("POST /agrilog-server/insert-byte.php HTTP/1.1");
        client.println("Host: 192.168.1.2:8085");
        client.println("Connection: Keep-Alive");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println("Content-Length: " + String(queryString.length()));
        client.println(); // end HTTP header
        client.println(queryString);

        Heltec.display->drawString(0, 48, "Packet sent.");
        Heltec.display->display();  
      }
      delay(5000);
    } else {
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Packet received.");
      Heltec.display->drawString(0, 10, "Less/more than 15 bytes.");
      Heltec.display->display();
    }
  }
}
