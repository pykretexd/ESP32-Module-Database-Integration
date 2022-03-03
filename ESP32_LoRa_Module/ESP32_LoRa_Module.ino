#include "heltec.h"
#include "images.h"
#include "WiFi.h"

char ssid[] = "Agrilog";
char pass[] = "12345678";
char server[] = "192.168.1.2";
int wifi_status = WL_IDLE_STATUS;

char outputString[80];

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

WiFiClient client;

void setup()
{
  Heltec.begin(true, true, true, true, BAND);
  Heltec.display->init();
  Heltec.display->flipScreenVertically();
  Heltec.display->setFont(ArialMT_Plain_10);
  logo();
  delay(500);

  LoRa.setFrequency(433920000);
  LoRa.setSyncWord(0x34);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(5);
  LoRa.setSignalBandwidth(500000);
  LoRa.setPreambleLength(6);
  LoRa.enableCrc();

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

  if (packetSize) {
    Heltec.display->drawString(0, 10, "Looping ...");
    Heltec.display->display();
    if (packetSize == 15) {
      Heltec.display->clear();
      rssi = "RSSI " + String(LoRa.packetRssi(), DEC);
      Heltec.display->drawString(0, 0, rssi);
      Heltec.display->drawString(0, 15, "Received packet:");

      for (int i = 0; i < packetSize; i++) {
        packet += (char) LoRa.read();
      }
      for (int i = 0; i < 16; i++) {
        Heltec.display->drawString(i * 6, 26, String(packet[i], HEX));
        i++;
      }
      Heltec.display->display();

      if (client.connect(server, 8085))
      {
        // Data
        int avg = (packet[2] & 0x01) + packet[3];
        int gust = (packet[2] & 0x03 >> 1) + packet[4];
        int windDirection = (packet[2] & 0x07 >> 2) + packet[5];
        int rainfall = packet[6] + packet[7];
        int light = (packet[8] >> 4 & 0x01) + (packet[8] >> 3 & 0x03) + packet[11] + packet[12];
        int temperature = ((((((packet[8] & 0x0f) * 256) + packet[9]) - 400) / 10) - 32) / 1.8;
        int stationName = (packet[1] & 0x0f) + (packet[2] & 0xf0);
        String queryString = String("device=") + String(packet[0], HEX) +
                             String("&station_name=") + String(stationName, HEX) +
                             String("&average_wind_speed=") + String(avg, DEC) +
                             String("&gust_wind_speed=") + String(gust, DEC) +
                             String("&wind_direction=") + String(windDirection, DEC) +
                             String("&rainfall=") + String(rainfall, DEC) +
                             String("&temperature=") + String(temperature, DEC) +
                             String("&humidity=") + String(packet[10], DEC) +
                             String("&light=") + String(light, DEC) +
                             String("&uvi=") + String(packet[13], DEC);

        client.println("POST /agrilog-server/insert-byte.php HTTP/1.1");
        client.println("Host: 192.168.1.2:8085");
        client.println("Connection: Keep-Alive");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.println("Content-Length: " + String(queryString.length()));
        client.println();
        client.println(queryString);

        Heltec.display->drawString(0, 48, "Packet sent.");
        Heltec.display->display();
        delay(5000);
        packetSize = 0;
        packet.clear();
      }
    } else {
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Packet received.");
      Heltec.display->drawString(0, 10, "Less/more than 15 bytes.");
      Heltec.display->display();
      delay(5000);
      packetSize = 0;
      packet.clear();
    }
  }
}
