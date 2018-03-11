
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>
#include "rgb_lcd.h"

rgb_lcd lcd;
const int pinBuzzer = 8;
const int pinLed   = 13;
const int pinTemp = A1;
const int B = 3975;

int treshold=100;
int state = 0;
int buz = 0;
int led = 0;
String msg ="";

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
  if (client.connect(server, 4300)) {
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
  pinMode(pinBuzzer,OUTPUT);
  pinMode(pinLed,OUTPUT);

    Serial.begin(9600);
      lcd.begin(16, 2);

    connect_();
        if (client.connect(server, 4300)) {
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
  
              // if you get a connection, report back via serial:
          if (client.connect(server, 4300)) {
            // Make a HTTP request:
            String req="GET /pause_card";
            client.println(req);
            client.println();
            client.stop();
            Serial.println("\nconnected\n");
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }
    delay(500);

    // if there are incoming bytes available
    // from the server, read them and print them:
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
      
      
      
/********* adding tempurature value to db **************/

      // Get the (raw) value of the temperature sensor.
      int val = analogRead(pinTemp);
      // Determine the current resistance of the thermistor based on the sensor value.
      float resistance = (float)(1023-val)*10000/val;
      // Calculate the temperature based on the resistance value.
      int temperature = 1/(log(resistance/10000)/B+1/298.15)-273.15;
      String ch = "temp:";
      ch+=temperature;

/******* displating message *******/ 

          // reset the display:
          lcd.clear();
          lcd.display();
          lcd.setCursor(0, 0);
          
          if (client.connect(server, 4300)) {
            // Make a HTTP request:
            String req="GET /getMsg";
            client.println(req);
            client.println();
            String resp="";
            while(client.connected() && client.available()){
                          char c = client.read();
                          resp=resp+c;         
            }
            resp = resp.substring(resp.lastIndexOf("\n")+1);
            Serial.println("msg : "+resp);
            lcd.print(resp);
           
            client.stop();
          } else {
            // if you didn't get a connection to the server:
            Serial.println("connection failed");
          }

/********** get theeshold value from database ***************/ 

  // if you get a connection, report back via serial:
          if (client.connect(server, 4300)) {
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
/********** if (tempurature>threshold) ***************/ 

  if(temperature>treshold) {
        // if you get a connection, report back via serial:
          if (client.connect(server, 4300)) {
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
  
/********** get led / buzzer state from database ***************/ 
          
            //Buzzer
            if (client.connect(server, 4300)) {
            // Make a HTTP request:
            String req="GET /getSensorState?sensor=buzzer";
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
            if (client.connect(server, 4300)) {
            // Make a HTTP request:
            String req="GET /getSensorState?sensor=led";
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
  }  
     
}

