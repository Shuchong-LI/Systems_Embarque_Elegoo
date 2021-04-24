// Import required libraries

#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
// WiFi parameters

const char* ssid = "Freebox-071359";
const char* password = "reputavero5#-digesserit97-fumeis-augendus";

const int rs = 12, en = 11, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en , d4, d5, d6, d7);

void connect_to_wifi()
{
  // Start Serial 
  Serial.begin(115200);
  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  // Print the IP address
  Serial.println(WiFi.localIP());
}
void setup(void)
{
  Serial.println("Try connect to Wifi");
  //connect_to_wifi();


  lcd.begin(16, 2);
  /*WiFiClient client;
  const int PORT = 7373;
  if(!client.connect("78.232.120.81", PORT))
  {
    Serial.println("connection failed");
    return; 
  }

  while (client.connected() || client.available())
  {
    if (client.available())
    {
      String line = client.readStringUntil('\n');
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print(line);
    }
  }*/
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("test");
}
void loop() 
{
  
}
