
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include "ESP8266WiFi.h"


const char* ssid = "your ssid"; //Enter SSID

const char* password = "password"; //Enter Password
const char* host = "api.pushingbox.com";
const char* devid = "your device id got from pushing box";

int value = 0;
int servo_angle=0;

Servo myservo;

#define rst D3
#define ss D2

#define RST_PIN         rst          // Configurable, see typical pin layout above
#define SS_PIN          ss         // Configurable, see typical pin layout above

#define motor D4



MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

 
byte accessUID[12] ={0x73, 0xBE ,0x6F, 0x1A};



void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
  myservo.attach(2,500,2400);
  myservo.write(0);
    
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
  delay(500);
  Serial.print("*");
  }

  Serial.println("");
  Serial.println("WiFi connection Successful");
  Serial.print("The IP Address of ESP8266 Module is: ");
  Serial.print(WiFi.localIP());// Print the IP address

}

void loop() {

	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	// Dump debug info about the card; PICC_HaltA() is automatically called
if((mfrc522.uid.uidByte[0] == accessUID[0] && mfrc522.uid.uidByte[1] == accessUID[1] && mfrc522.uid.uidByte[2] == accessUID[2] && mfrc522.uid.uidByte[3] == accessUID[3])){
  if(servo_angle==90){
    myservo.write(0);
    servo_angle=0;  
  }
  else{
    myservo.write(90);
    servo_angle=90; 
  }
  

  
    Serial.print("connecting to ");
    Serial.println(host);


    // Use WiFiClient class to create TCP connections
    WiFiClient client;
    const int httpPort = 80;
    if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
    }


    // We now create a URI for the request
    String url = "/pushingbox";
    url += "?devid=";
    url += devid;

    Serial.print("Requesting URL: ");
    Serial.println(url);


    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");
    unsigned long timeout = millis();
    while (client.available() == 0) {
        if (millis() - timeout > 5000) {
            Serial.println(">>> Client Timeout !");
            client.stop();
            return;
        }
    }


    // Read all the lines of the reply from server and print them to Serial
    while(client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
    }


    Serial.println();
    Serial.println("closing connection");

  }

mfrc522.PICC_HaltA();


}
