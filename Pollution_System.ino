//libraries
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SD.h>
#include <Wire.h>
#include <SPI.h>
#include "PMS.h"
#include <TFT.h>  // Arduino LCD library
#include <Multichannel_Gas_GMXXX.h>
GAS_GMXXX<TwoWire> gas;


// Set the pins used by the SD card module
const int chipSelect = 53;

// pin definition for the TFT
#define cs   10
#define dc   9
#define rst  8


// Create a TinyGPS++ object
TinyGPSPlus gps;

//PMS sensor
PMS pms(Serial2);
PMS::DATA data;

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);
int co_value, no2_value;

// Define NO2 sensor calibration values
float NO2_conversion_factor = 0.00015;

//to start 5 second countdown
unsigned long previousMillis = 0;
const unsigned long interval = 5000; //5 second interval


void setup() {
  // Start the serial communication
  Serial.begin(9600);
  //GPS
  Serial1.begin(9600);
  //PMS sensor
  Serial2.begin(9600);
  //Grove sensor
  gas.begin(Wire, 0x08);
  
  TFTscreen.begin();
  TFTscreen.setCursor(45, 50);
  //clear the screen with a black background
  TFTscreen.background(0, 0, 0);
  TFTscreen.print("Booting Up");
  delay(1000);
  
  TFTscreen.background(0, 0, 0);
  TFTscreen.setCursor(45, 50);
  TFTscreen.stroke(255,255,255);//white font
  TFTscreen.print("Outdoor Air");
  TFTscreen.setCursor(28, 60);
  TFTscreen.stroke(255,255,255);//white font
  TFTscreen.print("Pollution Monitoring");
  TFTscreen.setCursor(40, 70);
  TFTscreen.stroke(255,255,255);//black font
  TFTscreen.print("and Mapping System");
  delay(1000);

  // Initialize the SD card 
    pinMode(53, OUTPUT);
    if (!SD.begin(chipSelect)){
    //print a message to show sd card is detected or not
    Serial.println("SD card not detected");
    //clear the screen with a black background
    TFTscreen.background(0, 0, 0);
    TFTscreen.setCursor(25, 50);
    TFTscreen.stroke(155,255,255);//white font
    TFTscreen.print("SD card not detected");
    delay(1000);
  }
      else {
    Serial.println("SD card detected");
    //clear the screen with a black background
    TFTscreen.background(0, 0, 0);
    TFTscreen.setCursor(35, 50);
    TFTscreen.stroke(255,255,255);//white font
    TFTscreen.print("SD card detected");
    delay(1000);
  }
}

void loop() {
   unsigned long currentMillis = millis();
  // Read GPS data
  while (Serial1.available() > 0) {
    gps.encode(Serial1.read());
  }
  
  //CO and NO2 readings conversion
  co_value= gas.getGM702B();
  no2_value =  gas.getGM102B(); 
  const float co_calibration = 55.25;
  float NO2_conversion_factor = 0.00015;
  
  // Convert raw reading to voltage   
   float co_concentration = (co_value * 1.0 / 1023.0 * 1000.0)/ co_calibration;
   float no2_concentration = no2_value * NO2_conversion_factor;
 
  
//read the PMS sensor
  if(pms.read(data)){
    // clear the screen with a black background
    TFTscreen.background(0, 0, 0);
    TFTscreen.setCursor(0, 0);
    TFTscreen.stroke(255,255,255);//white font
    TFTscreen.print("Air Information:");
    TFTscreen.setCursor(0,5);
    // Change color based on PM1.0 reading
    TFTscreen.setCursor(0, 20);
    TFTscreen.stroke(255,255,255);
    TFTscreen.print("PM1.0: ");
    if (data.PM_AE_UG_1_0 < 5) {
      TFTscreen.stroke(0,255,0); //green font
      TFTscreen.print(String(data.PM_AE_UG_1_0)+"(ug/m3)= Good Air");
    } else if (data.PM_AE_UG_1_0 < 15) {
      TFTscreen.stroke(255,0,0); //blue font
      TFTscreen.print(String(data.PM_AE_UG_1_0)+"(ug/m3)= Moderate");

    } else {
      TFTscreen.stroke(0,0,255);//red font
      TFTscreen.print(String(data.PM_AE_UG_1_0)+"(ug/m3)= Poor Air");
    }
    

    // Change color based on PM2.5 reading
    TFTscreen.setCursor(0, 30);
    TFTscreen.stroke(255,255,255);
    TFTscreen.print("PM2.5: ");
    
    if (data.PM_AE_UG_2_5 < 35) {
    TFTscreen.stroke(0,255,0);
    TFTscreen.print(String(data.PM_AE_UG_2_5)+"(ug/m3)=Good Air");
     }   else if (data.PM_AE_UG_2_5 < 41) {
    TFTscreen.stroke(255,0,0);//blue
    TFTscreen.print(String(data.PM_AE_UG_2_5)+"(ug/m3)= Moderate"); 
    }
    else {
    TFTscreen.stroke(0,0,255);
    TFTscreen.print(String(data.PM_AE_UG_2_5)+"(ug/m3)= Poor Air");
      }

    // Change color based on PM10 reading
    TFTscreen.setCursor(0, 40);
    TFTscreen.stroke(255,255,255);
    TFTscreen.print("PM10: ");
    if (data.PM_AE_UG_10_0 < 30) {
    TFTscreen.stroke(0,255,0); //green font
    TFTscreen.print(String(data.PM_AE_UG_10_0)+"(ug/m3)= Good Air");
        }   else if (data.PM_AE_UG_10_0 < 54) {
    TFTscreen.stroke(255,0,0);//blue font
    TFTscreen.print(String(data.PM_AE_UG_10_0)+"(ug/m3)= Moderate");
        }   else {
    TFTscreen.stroke(0,0,255); //red font
    TFTscreen.print(String(data.PM_AE_UG_10_0)+"(ug/m3)= Poor Air");
          }

    //Grove multichannel sensor CO
    TFTscreen.setCursor(0, 50);
    TFTscreen.stroke(255,255,255);
    TFTscreen.print("CO: ");

   if (co_concentration < 9) {
    TFTscreen.stroke(0, 255, 0); //green font
    TFTscreen.print(co_concentration, 2);
    TFTscreen.print("ppm = Good Air ");
        } 
    else if (co_concentration < 15) {
    TFTscreen.stroke(255, 0, 0);//blue font
    TFTscreen.print(co_concentration, 2);
    TFTscreen.print(" ppm = Moderate ");
      } 
      else {
    TFTscreen.stroke(0, 0, 255); //red font
    TFTscreen.print(co_concentration, 2);
    TFTscreen.print("ppm = Poor Air");
      }

    //Grove sensor NO2
    TFTscreen.setCursor(0, 60);
    TFTscreen.stroke(255,255,255); //white Font
    TFTscreen.print("NO2: ");

 if (no2_concentration < 0.1) {
    TFTscreen.stroke(0, 255, 0); //green font
    TFTscreen.print(no2_concentration, 2);
    TFTscreen.print(" ppm = Good Air ");
      } 
else if (no2_concentration < 0.9) {
    TFTscreen.stroke(255, 0, 0);//blue font
    TFTscreen.print(no2_concentration, 2);
    TFTscreen.print(" ppm = Moderate ");
      } 
        else {
    TFTscreen.stroke(0, 0, 255); //red font
    TFTscreen.print(no2_concentration, 2);
    TFTscreen.print(" ppm = Poor Air ");
     delay(100);
    }


  // Check if new GPS data is available
  if (gps.location.isUpdated()) {
    Serial.println("available");
  }
    else{
    Serial.println("not available");
  }

   // check if 5 second has passed
  if (currentMillis - previousMillis >= interval) {
    // open data file
   File dataFile = SD.open("AIRDATA.txt", FILE_WRITE);
   
    if (dataFile) {
     if (dataFile.size() == 0) {
    // Write the header line onn the text file.
    dataFile.println("latitude,  longitude,  date, time, PM1.0, PM2.5, PM10,  CO,  NO2");
  }

      // Write the location data to the file
      dataFile.print(gps.location.lat(), 6);
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(gps.location.lng(), 6);
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(gps.date.year());
      dataFile.print("/");
      dataFile.print(gps.date.month());
      dataFile.print("/");
      dataFile.print(gps.date.day());
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(gps.time.hour());
      dataFile.print(":");
      dataFile.print(gps.time.minute());
      dataFile.print(":");
      dataFile.print(gps.time.second());
      dataFile.print(" ");
      dataFile.print(String(data.PM_AE_UG_1_0));
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(String(data.PM_AE_UG_2_5));
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(String(data.PM_AE_UG_10_0));
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(co_concentration);
      dataFile.print(",");
      dataFile.print(" ");
      dataFile.print(no2_concentration);
      dataFile.println();
      dataFile.flush();
      // Close the file
      dataFile.close();
      // Print a message to serial indicating successful write
      Serial.println("Data is being written to SD card:)");
          } else {
      // Print an error message to serial
      Serial.println("Error opening file");
       }
     // update previousMillis variable
    previousMillis = currentMillis;
  }
    
  }
}
