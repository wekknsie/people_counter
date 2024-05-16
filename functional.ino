#include <SPI.h>
#include <Ethernet.h>

volatile int numOfPeople = 0;
const int in = 2, out = 3;

byte mac[] = {0x10, 0x7B, 0x44, 0xF2, 0xCC, 0xCD};
EthernetServer server(80);

String webPage = R"=====(
<!DOCTYPE html>
<html lang="cs">
<head>
<meta charset="UTF-8">
<title>Watcher</title>
</head>
<main>
<h1>Aktuální počet lidí v objektu:</h1>
<span id="NumOfPeople">N/A</span>
<script>
function getNumOfPeople(){
var INOrequest = new XMLHttpRequest();
INOrequest.onreadystatechange = function(){
if(this.readyState == 4 && this.status && this.responseText != null){
document.getElementById("NumOfPeople").innerHTML = this.responseText;
}
};
INOrequest.open("GET", "readCount", true);
INOrequest.send();
}
setInterval(getNumOfPeople, 1000);
</script>
</main>
</html>
)=====";

String settings = "settings", readCount = "readCount";

void add();
void sub();
void updateCounter();

int oldIn = 1, oldOut = 1, currIn, currOut;

void setup(){
  Serial.begin(9600);

  //attachInterrupt(digitalPinToInterrupt(in), add, FALLING);
  //attachInterrupt(digitalPinToInterrupt(out), sub, FALLING);

  pinMode(in, INPUT_PULLUP);
  pinMode(out, INPUT_PULLUP);
    
  Ethernet.begin(mac);
  server.begin();
  Serial.print(F("Server Started...\nLocal IP: "));
  Serial.println(Ethernet.localIP());
}

void loop(){
  Serial.println(numOfPeople);  
  String HTTP_req;
  EthernetClient client = server.available();
  updateCounter();
  if(client){
    bool currentLineIsBlank = true;
    while(client.connected()){
      if(client.available()){
      char c = client.read();
      HTTP_req += c;
      if(c == '\n' && currentLineIsBlank){          
        Serial.println(HTTP_req);
        Serial.print("\n\n");
        Serial.print(HTTP_req.indexOf(readCount));
        Serial.print("\n");
        if(HTTP_req.indexOf(readCount) > -1){
          Serial.print("READCOUNT");
          client.println(numOfPeople);
        }else if(HTTP_req.indexOf(settings) >- 1){
          Serial.print("SETTINGS");
        }else{
          Serial.print("NORMAL");
          client.println("HTTP/1.1 200 OK\n\rContent-Type: text/html\n\r\n\r");
          client.println(webPage);
        }
        HTTP_req = "";
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

void add(){
  /*static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  if(abs(interruptTime - lastInterruptTime) > 500){
    numOfPeople++;
  }
  lastInterruptTime = interruptTime;*/
  numOfPeople += 1;
}

void sub(){
  numOfPeople--;
}

void updateCounter(){
  static unsigned long lastInterruptTime = 0;
  unsigned long interruptTime = millis();
  currIn = digitalRead(in);
  currOut = digitalRead(out);
  if(currIn == 0 && oldIn == 1 && abs(interruptTime - lastInterruptTime) > 200){
    add();
    lastInterruptTime = interruptTime;
  }else if(currOut == LOW && oldOut == HIGH && abs(interruptTime - lastInterruptTime) > 200){
    sub();
    lastInterruptTime = interruptTime;
  }
  oldIn = currIn;
  oldOut = currOut;
}