#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define FIREBASE_HOST "sistem-kehadiran-rfid.firebaseio.com" 
#define FIREBASE_AUTH "pTRnR3gKIlvakCbEFRoTFLveE1qJAat60A7ooVYD" 

#define WIFI_SSID "azulgranas_id" 
#define WIFI_PASSWORD "30297208"

#define SS_PIN D4
#define RST_PIN D2       // Configurable, see typical pin layout above

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

String idCard = "";
int count = 0;

void setup() {
  Serial.begin (9600);

  delay (1000);

  // connect to Wifi
  WiFi.begin (WIFI_SSID, WIFI_PASSWORD); 
  Serial.print ("Connecting to ");
  Serial.print (WIFI_SSID);
  while (WiFi.status()!= WL_CONNECTED) {
    Serial.print (".");
    delay (500);
  }
  Serial.println ();
  Serial.print ("Connected to : ");
  Serial.print (" ");
  Serial.println (WIFI_SSID);
  Serial.print ("IP Address is:");
  Serial.println (WiFi.localIP ()); 
  Firebase.begin (FIREBASE_HOST, FIREBASE_AUTH);

  SPI.begin();          // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522 module
  Serial.println("RFID reading UID (Please tap the card...)"); 
}

void loop() {
  if ( mfrc522.PICC_IsNewCardPresent()) {
    if ( mfrc522.PICC_ReadCardSerial()) {
      Serial.print("Tag UID:");
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        // Serial.print(mfrc522.uid.uidByte[i], HEX);
        idCard = idCard + mfrc522.uid.uidByte[i];
      }
      Serial.print(idCard);
      Firebase.pushString ("id-kartu", idCard); // push data card id to database
      Serial.println();
      mfrc522.PICC_HaltA();
      idCard = "";

      // check connection with firebase
      if (Firebase.failed()){
        Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
        Serial.println(Firebase.error());
        delay(10);
        Serial.println("Error connecting firebase!");
        count++;
        if (count == 10) {
          count = 0;
          ESP.reset();
        }
        return;
      }
    }
  }
}