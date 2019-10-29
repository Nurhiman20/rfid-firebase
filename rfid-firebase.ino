#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <SPI.h>
#include <MFRC522.h>

#define FIREBASE_HOST "xxxxxxxxxxxxxxxxxxxx" 
#define FIREBASE_AUTH "xxxxxxxxxxxxxxxxxxx" 

#define WIFI_SSID "azulgranas_id" 
#define WIFI_PASSWORD "xxxxxxxxx"

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

      String pathNama = "attendance/" + idCard + "/nama";
      String pathJabatan = "attendance/" + idCard + "/jabatan";

      Serial.print(idCard);
      Serial.print(" ");
      Serial.print(Firebase.getString(pathNama)); // get data from database
      Serial.print(" (");
      Serial.print(Firebase.getString(pathJabatan));  // get data from database
      Serial.println(")");
      
      // Firebase.pushString ("id-kartu", idCard); // push data card id to database
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