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

  LoRa.setFrequency( 433920000 );
  //LoRa.setSyncWord(0xF3);
  LoRa.setSyncWord(0x34);
  LoRa.setSpreadingFactor(7);
  LoRa.setCodingRate4(5);
  LoRa.setSignalBandwidth( 500000 );
  LoRa.setPreambleLength( 6 );
  LoRa.enableCrc();

  // Initialization success
  Heltec.display->clear();
  Heltec.display->drawString(0, 0, "Heltec.LoRa Initial success!");
  Heltec.display->drawString(0, 10, "Wait for incoming data...");
  Heltec.display->display();
  
  delay(1000);

  // Attempt to connect to WiFi.
  while( wifi_status != WL_CONNECTED )
  {
    static int counter = 0;

          sprintf(output_string, "Attempts: %d", counter++);
          Heltec.display->clear();
          Heltec.display->drawString(0, 0, output_string);
          Heltec.display->display();
          wifi_status = WiFi.begin(ssid, pass);

    // Wait 5 sec
    delay(5000);
  }
  
  // Check if connection failed.
  if ( wifi_status != WL_CONNECTED) 
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
  }
}

void loop()
{
  if (client.connect(server, 8085)) 
    {
         Heltec.display->clear();
         Heltec.display->drawString(0, 0, "Port 8085 agrilog");
         Heltec.display->display();

      // Data
      String queryString = String("user=") + String("1") +
                           String("&station=") + String("1") +
                           String("&average_wind_speed=") + String("12") + 
                           String("&gust_wind_speed=") + String("13") +
                           String("&wind_direction=") + String("180") + 
                           String("&rainfall=") + String("101") +
                           String("&temperature=") + String("41") +
                           String("&humidity=") + String("31") +
                           String("&light=") + String("13") +
                           String("&uvi=") + String("5");

      // Send to server through a HTTP POST request.
      client.println("POST /agrilog-server/insert-temp.php HTTP/1.1");
      client.println("Host: 192.168.1.2:8085");
      client.println("Connection: Keep-Alive");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-Length: " + String(queryString.length()));
      client.println(); // end HTTP header

      client.println(queryString);

      // Confirm connection.
      Heltec.display->clear();
      Heltec.display->drawString(0, 0, "Connection established.");
      Heltec.display->drawString(0, 10, "Sending packets...");
      Heltec.display->display();

      delay(10000);
    }
}
