/**
   http_8x8_client.ino

    Created on: 24.05.2015

*/

#include <Arduino.h>

#include <LEDMatrixDriver.hpp>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include <time.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;


int pinCS = D4;
int numberOfHorizontalDisplays = 2;
int numberOfVerticalDisplays = 1;
char time_value[20];
// LED Matrix Pin -> ESP8266 Pin
// Vcc -> 3v (3V on NodeMCU 3V3 on WEMOS)
// Gnd -> Gnd (G on NodeMCU)
// DIN -> D7 (Same Pin for WEMOS)
// CS -> D4 (Same Pin for WEMOS)
// CLK -> D5 (Same Pin for WEMOS)

Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

int wait = 300; // In milliseconds

int spacer = 1;
int font_width = 6;
int width = font_width + spacer; // The font width is 5 pixels

char api_server[] = "http://yourapiserver:8888/bus/v1/arrive_all/";

byte ch_now[8]    = {0x78,0x08,0x28,0xfc,0x00,0x78,0x40,0x78 }; // 곧 n
byte ch_arr1[8] = {0x3e,0x20,0x20,0x3e,0x00,0x08,0x08,0x7f }; // 도
byte ch_arr2[8] = {0x12,0x7a,0x13,0x6a,0x00,0x7e,0x02,0x02}; // 착
byte ch_after[8] = {0x20,0xf8,0x70,0x88,0x70,0x00,0xf8,0x20}; // 후 a
byte ch_min[8] = {0x48,0x78,0x48,0x78,0xfc,0x10,0x40,0x78}; // 분 m
byte ch_sec[8] = {0x20,0xf8,0x50,0x88,0x20,0x20,0xf8,0x00}; // 초 s

byte ch_super0[8]= {0x30,0x30,0x48,0x84,0x00,0xfc,0x48,0x48};// 슈 x
byte ch_super1[8]= {0x04,0xf4,0x64,0x6c,0x64,0xf4,0x04,0x04};//퍼 y

byte ch_kyungro0[8]= {0xe4,0x3c,0x64,0xdc,0x04,0x78,0x84,0x78};//경 o
byte ch_kyungro1[8]= {0xf8,0x08,0xf8,0x80,0xf8,0x20,0x20,0xfc};//로 d

String getData() {
  String info = "00";
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");
    // configure traged server and url

    http.begin(api_server); //HTTP

    USE_SERIAL.print("[HTTP] GET...\n");
    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
      USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

      // file found at server
      if (httpCode == HTTP_CODE_OK) {
        info = http.getString();
      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  return info;
}

void setup() {

  USE_SERIAL.begin(115200);
  // USE_SERIAL.setDebugOutput(true);


  //
  //  for (uint8_t t = 4; t > 0; t--) {
  //    USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
  //    USE_SERIAL.flush();
  //    delay(1000);
  //  }
  //
   WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("seo-ap", "gogofish");
  //WiFiMulti.addAP("seo_egg_5GHz", "archi2011");
  ////i.style cafe 2G
  //// WiFiMulti.addAP("i.style cafe 2G", "01234567");

  //WiFiMulti.addAP("KT_GiGA_2G_Wave2_36C1","6dbbaxg502");
  
  //WiFi.begin("seo-ap", "gogofish"); // Initiate connection to the Wi-Fi network
 
  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.println(".");
  }
  Serial.println("Connected!");
  delay(1000);
  configTime(28800, 0, "3.asia.pool.ntp.org", "time.nist.gov");
  //matrix.setIntensity(3); // Use a value between 0 and 15 for brightness
  //matrix.setRotation(0, 1); // The first display is position upside down
  //matrix.setRotation(1, 1); // The first display is position upside down
  //matrix.setRotation(2, 1); // The first display is position upside down
  //matrix.setRotation(3, 1); // The first display is position upside down


}

void loop() {

  matrix.fillScreen(LOW);
  time_t now = time(nullptr);
  String time = String(ctime(&now));
  time.trim();
  //  time.substring(11, 19).toCharArray(time_value, 10);
  //  matrix.drawChar(2, 0, time_value[0], HIGH, LOW, 1); // H
  //  matrix.drawChar(8, 0, time_value[1], HIGH, LOW, 1); // HH
  //  matrix.drawChar(14, 0, time_value[2], HIGH, LOW, 1); // HH:
  //  matrix.drawChar(20, 0, time_value[3], HIGH, LOW, 1); // HH:M
  //  matrix.drawChar(26, 0, time_value[4], HIGH, LOW, 1); // HH:MM


  // matrix.write(); // Send bitmap to display
  delay(10000);
  String info = getData();
  //String info = "0123456789";
  display_message(info); // Display time in format 'Wed, Mar 01 16:03:20 2017
}

void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  byte mask = B10000000;
  for ( int iy = 0; iy < height; iy++ )
  {
    for ( int ix = 0; ix < width; ix++ )
    {
      matrix.drawPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));
      mask = mask >> 1;
    }
    mask = B10000000;
  }
}

void display_message(String message) {
   Serial.println(message);
  for ( int i = 0 ; i < width * message.length() + matrix.width() - spacer; i++ ) {
    matrix.fillScreen(LOW);
    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < message.length() ) {
        // check m, s, n --> draw sprite
 //           matrix.drawChar(x, y, message[letter], HIGH, LOW, 1);
        switch (message[letter]) {
          case 'm':
            drawSprite((byte*)&ch_min, x, 0, font_width, 8);
            break;
          case 's':
            drawSprite((byte*)&ch_sec, x, 0, font_width, 8);
            break;
          case 'n':
            drawSprite((byte*)&ch_now, x, 0, font_width, 8);
            break;
          case 'a':
            drawSprite((byte*)&ch_after, x, 0, font_width, 8);
            break;
          case 'b':
            drawSprite((byte*)&ch_arr1, x, 0, font_width, 8);
            break;  
          case 'c':
            drawSprite((byte*)&ch_arr2, x, 0,font_width, 8);
            break;
          case 'x':
            drawSprite((byte*)&ch_super0, x, 0, font_width, 8);
            break;
          case 'y':
            drawSprite((byte*)&ch_super1, x, 0, font_width, 8);
            break;
           case 'o':
            drawSprite((byte*)&ch_kyungro0, x, 0, font_width, 8);
            break;
          case 'd':
            drawSprite((byte*)&ch_kyungro1, x, 0, font_width, 8);
            break;
          default:
            matrix.drawChar(x, y, message[letter], HIGH, LOW, 1); // HIGH LOW means foreground ON, background off, reverse to invert the image
        }

      }
      letter--;
      x -= width;
    }
    matrix.write(); // Send bitmap to display
    delay(wait / 2);
  }
}
