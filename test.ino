#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>

File HMTL_file;

int numOfPeople = 0;
const int = 11, out = 12;

byte mac[] = {0x90, 0xA2, 0xDA, 0x00, 0x4A, 0xE0}; //TODO
EthernetServer server(80);

void setup(){
    Serial.begin(9600);
    pinMode(11, INPUT);
    pinMode(12, INPUT);

    Serial.println(F("Initializing SD card..."));
    if(!SD.begin(4))
    {
        Serial.println(F("Initialization failed!")); 
        return -1;
    }
    Serial.println(F("Initialization OK"));

    if(!SD.exists("index.html"))
    {
        Serial.println(F("index.html not found!"));
        return -1;
    }
    Serial.println(F("index.html found"));

    Ethernet.begin(mac);
    server.begin();
    Serial.print(F("Server Started...\nLocal IP: "));
    Serial.println(Ethernet.localIP());
}

void loop(){
    String HTTP_req;
    EthernetClient client = server.available();

    if(client){
        bool currentLineIsBlank = true;
        while(client.connected()){
            if(client.available()){
                char c = client.read();
                HTTP_req += c;
                if(c == '\n' && currentLineIsBlank){
                    client.println("HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r");

                    if(HTTP_req.indexOf("readCount")>-1){
                        client.println(numOfPeople);
                    }else if(HTTP_req.indexOf("settings")>-1){
                        /*HTML_file = SD.open("settings.html");
                        if(HTML_file){
                            while(HTML_file.available()){
                                client.write(HTML_file.read());
                            }
                            HMTL_file.close();
                        }*/
                    }else{
                        HTML_file = SD.open("index.html");
                        if(HTML_file){
                            while(HTML_file.available()){
                                client.write(HTML_file.read());
                            }
                            HMTL_file.close();
                        }
                    }
                    break;
                }
                if(c == '\n'){
                    currentLineIsBlank = true;
                }else if(c != '\r'){
                    currentLineIsBlank = false;
                }
            }
        }
        delay(10);
        client.stop();
    }
}