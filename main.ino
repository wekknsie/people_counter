#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <DHT.h>

File HMTL_file;

byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x4A, 0xE0};
EthernetServer server(80);

void setup()
{
  Serial.begin(9600);
  dht.begin();
  pinMode(53, OUTPUT);
  //-----------------------------------------------------
  // Kontrola SD karty
  //-----------------------------------------------------
  Serial.println(F("Initializing SD card..."));
  if(!SD.begin(4))
  {
    Serial.println(F("Initialization failed!")); return;
  }
  Serial.println(F("Initialization OK"));

  if(!SD.exists("index.html"))
  {
    Serial.println(F("index.html not found!")); return;
  }
  Serial.println(F("index.html found"));
  //-----------------------------------------------------
  Ethernet.begin(mac);
  server.begin();
  Serial.print(F("Server Started...\nLocal IP: "));
  Serial.println(Ethernet.localIP());
}
//==================================================================================
void loop()
{
  String HTTP_req;
  EthernetClient client = server.available();
  //---------------------------------------------------------------------------
  if(client)
  {
    boolean currentLineIsBlank = true;
    while (client.connected())
    {
      if(client.available())
      { 
        char c = client.read();
        HTTP_req += c;
        if(c == '\n' && currentLineIsBlank)
        {
          client.println("HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r");
          //-------------------------------------------------------------------
          if(HTTP_req.indexOf("readDHT11")>-1){
            client.println("Temp&nbsp;&nbsp;&nbsp;&nbsp;: ");
            client.println(dht.readTemperature());
            client.println("°C<br>Humidity:&nbsp;&nbsp;&nbsp;");
            client.println(int(dht.readHumidity()));
            client.println("&nbsp;%");
          }
          //-------------------------------------------------------------------
          else{
            HMTL_file = SD.open("index.html");
            if(HMTL_file)
            {
              while(HMTL_file.available()) client.write(HMTL_file.read());
              HMTL_file.close();
            }
          }
          //-------------------------------------------------------------------
          //HTTP_req = "";
          break;
        }
        //---------------------------------------------------------------------
        if(c == '\n') currentLineIsBlank = true;
        else if(c != '\r') currentLineIsBlank = false;
      }
    }
    delay(10);
    client.stop();
  }
}