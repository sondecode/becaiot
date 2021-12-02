/*
 * IoT ESP8266 Based Mood Lamp (RGB LED) Controller Program
 * https://circuits4you.com
 */
 
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <OneWire.h>
#include "control.h"

#define ServoPort D3  
//SSID and Password of your WiFi router
const char* ssid = "MI8SE";
const char* password = "11112222";

Servo myservo; 
ESP8266WebServer server(80);
OneWire  ds(2);

//LED Connections
const int BlueLED = 16; // D0
const int RedLED = 5;    // D1 
const int GreenLED = 4; // D2

//=======================================================================
//                    handles main page
//=======================================================================
void handleRoot() {
  Serial.println("Root Page Requested");
  server.send(200, "text/html", MAIN_page);
}

//=======================================================================
//                    Handle Set Color
//=======================================================================
void handleForm() {
  //Saperate Colors are sent through javascript
  String red = server.arg("r");
  String green = server.arg("g");
  String blue = server.arg("b");
  int r = red.toInt();
  int g = green.toInt();
  int b = blue.toInt();

  Serial.print("Red:");Serial.println(r);
  Serial.print("Green:");Serial.println(g);
  Serial.print("Blue:");Serial.println(b);
  
  //PWM Correction 8-bit to 10-bit
    /*
  r = r * 4; 
  g = g * 4;
  b = b * 4;
  */

  //for ULN2003 or Common Cathode RGB Led not needed
  /*
  r = 1024 - r;
  g = 1024 - g;
  b = 1024 - b;
  */
  //ESP supports analogWrite All IOs are PWM
  analogWrite(RedLED,r);
  analogWrite(GreenLED,g);
  analogWrite(BlueLED,b);

  server.sendHeader("Location", "/");
  server.send(302, "text/plain", "Updated-- Press Back Button");
 
  delay(500);  
}
void handleServo(){

  String POS = server.arg("servoPOS");

  int pos = POS.toInt();

  myservo.write(pos);   //--> Move the servo motor according to the POS value

  delay(15);

  Serial.print("Servo Angle:");

  Serial.println(pos);

  server.send(200, "text/plane","");

}
void handleFeed(){

  myservo.write(110);   //--> Move the servo motor according to the POS value

  delay(1000);

  myservo.write(45);

  server.send(200, "text/plane","");

}
void handleTurnoff(){

  analogWrite(RedLED,LOW);
  analogWrite(GreenLED,LOW);
  analogWrite(BlueLED,LOW);

  delay(1000);

  server.send(200, "text/plane","");

}
void handleTemp(){

    
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
   String result;
  
  if ( !ds.search(addr)) {
  Serial.println("No more addresses.");
  Serial.println();
  ds.reset_search();
  delay(250);
  return;
  }
  
  Serial.print("ROM =");
  for ( i = 0; i < 8; i++) {
  Serial.write(' ');
  Serial.print(addr[i], HEX);
  }
  
  if (OneWire::crc8(addr, 7) != addr[7]) {
  Serial.println("CRC is not valid!");
  return;
  }
  Serial.println();
  
  // the first ROM byte indicates which chip
  switch (addr[0]) {
  case 0x10:
  Serial.println("  Chip = DS18S20");  // or old DS1820
  type_s = 1;
  break;
  case 0x28:
  Serial.println("  Chip = DS18B20");
  type_s = 0;
  break;
  case 0x22:
  Serial.println("  Chip = DS1822");
  type_s = 0;
  break;
  default:
  Serial.println("Device is not a DS18x20 family device.");
  return;
  }
  
  ds.reset();
  ds.select(addr);
  ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);
  ds.write(0xBE);         // Read Scratchpad
  
  Serial.print("  Data = ");
  Serial.print(present, HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
  data[i] = ds.read();
  Serial.print(data[i], HEX);
  Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();
  
  int16_t raw = (data[1] << 8) | data[0];
  if (type_s) {
  raw = raw << 3; // 9 bit resolution default
  if (data[7] == 0x10) {
  // "count remain" gives full 12 bit resolution
  raw = (raw & 0xFFF0) + 12 - data[6];
  }
  } else {
  byte cfg = (data[4] & 0x60);
  // at lower res, the low bits are undefined, so let's zero them
  if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
  else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
  else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
  //// default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");

  result = "<html><head><meta http-equiv=\"refresh\" content=\"2\"> <body bgcolor=\"#add8e6\"><p style='text-align: center;'><span style='font-size: x-large;'><strong>ESP8266 and DS18B20 Temperature Sensor Server</strong></span></p><p style='text-align: center;'><span style='color: #0000ff;'><strong style='font-size: 70;'>"
                + String(celsius) +
                "<sup>o</sup>C</p></strong></span></body></html>";
  server.send(200, "text/html", result);
  delay(5000);
}
//=======================================================================
//                    SETUP
//=======================================================================
void setup(){
  Serial.begin(115200);   //Start serial connection  
  delay(500);
  myservo.attach(ServoPort);
  
  pinMode(RedLED,OUTPUT);
  pinMode(GreenLED,OUTPUT);
  pinMode(BlueLED,OUTPUT);
  
  WiFi.mode(WIFI_STA);
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("connecting");
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");  
  Serial.println("WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  
  server.on("/", handleRoot);  //Associate handler function to path
  server.on("/setRGB",handleForm);
  server.on("/setPOS",handleServo);
  server.on("/feed",handleFeed);
  server.on("/turnoff",handleTurnoff);
  server.on("/temp",handleTemp);
    
  server.begin();                           //Start server
  Serial.println("HTTP server started");
}
//=======================================================================
//                    LOOP
//=======================================================================
void loop(){
  server.handleClient();
}
