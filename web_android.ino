/*
  Web client

 This sketch connects to a website (http://www.google.com)
 using an Arduino Wiznet Ethernet shield.

 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13

 created 18 Dec 2009
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe, based on work by Adrian McEwen

 */

#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
const int buzzer = 8;
const int pinLight = A0;
const int pinLed   = 13;
const int pinTemp = A1;
const int pinReset = 4;
const int pinPause =3;
const int micro = A2;
const int B = 3975;
int seuil=100;
int sound_seuil=100;
int etat = 0;
int buz = 0;
int led = 0;
bool paused = false;
String msg ="";
String soundv="";

int stringStart, stringStop = 0;
int scrollCursor = 16;

byte mac[] = { 0x00, 0xAD, 0xA2, 0xAD, 0xA0, 0xAD };
int thresholdvalue = 400;

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 0, 89);
IPAddress server(172, 18, 46, 141);
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

void connect_(){
  
    while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  Serial.println("\n Connecting ...");
  
  system("ifdown eth0");
  delay(500);
  system("ifup eth0");
  // Open serial communications and wait for port to open:



  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }

  
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting to internet...");
  Serial.println(Ethernet.localIP());
  

  // if you get a connection, report back via serial:
  if (client.connect(server, 1337)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("GET /get_all_alerts");
    client.println();

  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
    client.stop();
}

void setup() {
  pinMode(pinReset,INPUT);
  pinMode(pinPause,INPUT);  
  pinMode(buzzer,OUTPUT);
  pinMode(pinLed,OUTPUT);

    Serial.begin(9600);
      lcd.begin(16, 2);

    connect_();
        if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /start_card";
            client.println(req);
            client.println();
            client.stop();
        Serial.println("\nCard started sended !!!\n");
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
}

void loop() {
if(!paused){
  if(digitalRead(pinPause)==HIGH) {
        Serial.println("\nPaused !!!\n");
    paused = true;
              // if you get a connection, report back via serial:
          if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /pause_card";
            client.println(req);
            client.println();
            client.stop();
        Serial.println("\nPaused and sended !!!\n");
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
    delay(500);
  }
  else{

  
    // if there are incoming bytes available
    // from the server, read them and print them:
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  
  
  
     // Read the value of the light sensor. The light sensor is an analog sensor.
      int sensorValue = analogRead(pinLight);
  // Turn the LED on if the sensor value is below the threshold.
         // Get the (raw) value of the temperature sensor.
      int val = analogRead(pinTemp);
  
      // Determine the current resistance of the thermistor based on the sensor value.
      float resistance = (float)(1023-val)*10000/val;
  
      // Calculate the temperature based on the resistance value.
      int temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;
  
  
  
         // Turn off the display:
      lcd.clear();
      lcd.display();
 

      String ch = "temp:";
      ch+=temperature;
           lcd.setCursor(0, 0);

     String ch1 = " lum:";
     ch1+=sensorValue;
         // lcd.setCursor(0, 1);
                  lcd.print(ch+ch1);
  ///insert_alert_query?valeur=10&capteur=toto


                       // GET Message
          if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /get_last_message";
            client.println(req);
            client.println();
            String resp="";
            while(client.connected() && client.available()){
                          char c = client.read();
                          resp=resp+c;
                      
            }
            resp = resp.substring(resp.lastIndexOf("\n")+1);
            Serial.println("msg : "+resp);
           

            // lcd.print(resp);

           /*  lcd.print(resp.substring(stringStart,stringStop));
              if(stringStart == 0 && scrollCursor > 0){
                scrollCursor--;
                stringStop++;
              } else if (stringStart == stringStop){
                stringStart = stringStop = 0;
                scrollCursor = 16;
              } else if (stringStop == resp.length() && scrollCursor == 0) {
                stringStart++;
              } else {
                stringStart++;
                stringStop++;
              }*/

               lcd.setCursor(0, 1);

              
              lcd.print(resp.substring(stringStart,stringStart+16));
              stringStart = stringStart + 1;
            if(stringStart>resp.length()) {
                stringStart = 0;
               
                }

  
             
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }

  
  if(temperature>seuil) {
  
        // if you get a connection, report back via serial:
          if (client.connect(server, 1337)) {
            Serial.println("connected");
            // Make a HTTP request:
            String req="GET /insert_alert_query?valeur=";
            req+=temperature;
            req+="&capteur=Temp";
            Serial.println(req);
            client.println(req);
            client.println();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
  }
  
  
  if(sensorValue<100) {
  
        // if you get a connection, report back via serial:
          if (client.connect(server, 1337)) {
            Serial.println("connected");
            // Make a HTTP request:
            String req="GET /insert_alert_query?valeur=";
            req+=sensorValue;
            req+="&capteur=Lum";
            client.println(req);
            client.println();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
  }
  
  if(digitalRead(pinReset)==HIGH){
          // if you get a connection, report back via serial:
          if (client.connect(server, 1337)) {
            Serial.println("reset");
            // Make a HTTP request:
            String req="GET /empty_alerts";
            client.println(req);
            client.println();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
  }
  
          // if you get a connection, report back via serial:
          if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /get_last_seuil";
            client.println(req);
            client.println();
            String resp="";
            while(client.connected() && client.available()){
                          char c = client.read();
                          resp=resp+c;
                      
            }
            resp = resp.substring(resp.lastIndexOf("\n")+1);
            Serial.println(resp);
           
            seuil=resp.toInt();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
  
  
          
          // if you get a connection, report back via serial:
          if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /get_last_etat";
            client.println(req);
            client.println();
            String resp="";
            while(client.connected() && client.available()){
                          char c = client.read();
                          resp=resp+c;
                      
            }
            resp = resp.substring(resp.lastIndexOf("\n")+1);
            Serial.println("Etat : "+resp);
           
            etat=resp.toInt();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }

          //Read micro
                 soundv=String(analogRead(micro));
                 Serial.println("Sound : "+soundv);
  
           if(etat) {
                  // if you get a connection, report back via serial:
                if (client.connect(server, 1337)) {
                  Serial.println("connected");
                  // Make a HTTP request:
                  String req="GET /insert_valeurs?lum=";
                  req+=sensorValue;
                  req+="&temp=";
                  req+=temperature;
                  req+="&sound=";
                  req+=soundv;
                  client.println(req);
                  client.println();
                  client.stop();
                  Serial.println("---------\nRefreshed ------- \n");
                } else {
                  // if you didn't get a connection to the server:
                  Serial.println("connection failed");
                }
           }


            //Buzzer
            if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /get_buzzer_state";
            client.println(req);
            client.println();
            String resp="";
            while(client.connected() && client.available()){
                          char c = client.read();
                          resp=resp+c;
                      
            }
            resp = resp.substring(resp.lastIndexOf("\n")+1);
            Serial.println("buz : "+resp);
           
             buz=resp.toInt();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
            if(buz) digitalWrite(buzzer,HIGH);
            else
            digitalWrite(buzzer,LOW);

            
            //LED
            if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /get_led_state";
            client.println(req);
            client.println();
            String resp="";
            while(client.connected() && client.available()){
                          char c = client.read();
                          resp=resp+c;
                      
            }
            resp = resp.substring(resp.lastIndexOf("\n")+1);
            Serial.println("led : "+resp);
           
             led=resp.toInt();
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
            if(led) digitalWrite(pinLed,HIGH);
            else
            digitalWrite(pinLed,LOW);



           //ALWAYS INSERT 
                if (client.connect(server, 1337)) {
                  // Make a HTTP request:
                  String req="GET /insert_valeurs?lum=";
                  req+=sensorValue;
                  req+="&temp=";
                  req+=temperature;
                  req+="&sound=";
                  req+=soundv;
                  client.println(req);
                  client.println();
                  client.stop();
                } else {
                  // if you didn't get a connection to the server:
                  Serial.println("connection failed");
                }
  }
}
else {

delay(10);
  if(digitalRead(pinPause)==HIGH) {
    paused = false;
              if (client.connect(server, 1337)) {
            // Make a HTTP request:
            String req="GET /start_card";
            client.println(req);
            client.println();
            client.stop();
        Serial.println("\nUnpaused and sended !!!\n");
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
    delay(400);
  }
}

  
     
}

