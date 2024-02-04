
#include <ESP32Servo.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DFRobotDFPlayerMini.h"



#define uS_TO_S_FACTOR 1000000 /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 60 /* Time ESP32 will go to sleep (in seconds) */

// Define digital pins

#define SERVICE_UUID        "91e8811b-184e-4f7f-bf3e-c834af88d46f"
#define CHARACTERISTIC_UUID "7325577d-a662-4789-bc62-24dfb14e5759"

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows); 

DFRobotDFPlayerMini player;

Servo myservo;
int servoPin = 13;
bool deviceConnected = false;
BLEServer* pServer = NULL;

void Lcd_main()
{
  lcd.setCursor(4, 0); 
  lcd.print("FACEMASK");
  delay(100); 
  lcd.setCursor(4, 1); 
  lcd.print("DETECTOR");
       
}

void Lcd_no_facemask()
{
  lcd.setCursor(1, 0); 
  lcd.print(" NO FACEMASK");
    
}
void Lcd_come_in()
{
  lcd.setCursor(1, 0); 
  lcd.print("FACEMASK OK");
  
}


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};


class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String temp = pCharacteristic->getValue().c_str();
      Serial.println(" ");
      Serial.println(temp);
      lcd.clear();       
      if (temp == "0") {       
        Serial.println("Facemask OFF");   
         lcd.clear();                    
        Lcd_no_facemask();
           player.play(3);
          delay(3000);            
        for (int x = 178; x > 0; --x) {
          myservo.write(x);
          delay(20);
        }
        delay(10000);
        for (int x = 0; x < 179; x++) {
          myservo.write(x);
          delay(20);
        }      
         
      }
      else if (temp == "1") {
        Serial.println("Facemask ON");
     lcd.clear();        
    Lcd_come_in(); 
       player.play(1); 
         delay(10000);
        lcd.clear();          
       Lcd_main();
       delay(1000); 
           player.play(2);
           delay(5000);                         
            }
       else {
         //Serial.println("Main");          
        Lcd_main();
        delay(1000); 
        player.play(2);
       delay(5000);
      }
      //delay(500);
    }
};

void startBLE() {
  BLEDevice::init("Facemaskdispenser");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setValue("Hello From Dispenser 1");
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void setup(void)
{
  Serial.begin(9600);
  startBLE();
  Serial.println("BLE OK");
   lcd.init();                    
  lcd.backlight();
  lcd.clear(); 
  Lcd_main();
   Serial2.begin(9600); // Init serial port for DFPlayer Mini
  // Start communication with DFPlayer Mini
    Serial.println("Connecting to DFplayer");
  while (!player.begin(Serial2))
  {
      Serial.print(".");
      delay(1000);
  }
  Serial.println(" DFplayer connected!");
  player.volume(28); // Set volume to maximum (0 to 30).
  player.play(2);
  
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
  myservo.write(178); 
  
}

void loop(void)
{
  delay(100); 
}

