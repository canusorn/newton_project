/*
   -PZEM004T-
   5V - 5V
   GND - GND
   D3 - TX
   D4 - RX

   RPM-D5

   DHT22(1) - D6
   DHT22(2) - D7
*/
#include <ESP8266WiFi.h>
#include <PZEM004Tv30.h>
#include <Wire.h>  // Include Wire if you're using I2C
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

#define PIN_RESET -1
#define DC_JUMPER 0
#define BLYNK_PRINT Serial

MicroOLED oled(PIN_RESET, DC_JUMPER); //Example I2C declaration, uncomment if using I2C
PZEM004Tv30 pzem(D3, D4); //rx,tx pin  Software serial
//PZEM004Tv30 pzem(&Serial);    //Hardware serial
DHT dht1(D6, DHT22);
DHT dht2(D7, DHT22);

unsigned long previousMillis = 0;
float voltage, current, power, energy, frequency, pf;

int N;
uint32_t RPM;
uint64_t T_Detect, T_prevDetect, T_diff;
bool updateRPM;
unsigned long previousMillisRPM;

char ssid[] = "IoTbundle";
char pass[] = "IoTbundle";
char auth[] = "0lm76q2CVQc-QDh1tXz6Th618iIkHytu";

uint8_t logo_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xC0, 0xF0, 0xE0, 0x78, 0x38, 0x78, 0x3C, 0x1C, 0x3C, 0x1C, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1C, 0x3C, 0x1C, 0x3C, 0x78, 0x38, 0xF0, 0xE0, 0xF0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x01, 0x00, 0x00, 0xF0, 0xF8, 0x70, 0x3C, 0x3C, 0x1C, 0x1E, 0x1E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0E, 0x0E, 0x1E, 0x1E, 0x1E, 0x3C, 0x1C, 0x7C, 0x70, 0xF0, 0x70, 0x20, 0x01, 0x01, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x1E, 0x0F, 0x0F, 0x07, 0x87, 0x87, 0x07, 0x0F, 0x0F, 0x1E, 0x3E, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1F, 0x1F, 0x3F, 0x3F, 0x1F, 0x1F, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

BLYNK_WRITE(V7)
{
  int pinValue = param.asInt();
  if (pinValue == HIGH) {
    Serial.println("Reset energy");
    pzem.resetEnergy();
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(D5, INPUT);
  Wire.begin();

  //------Display LOGO at start------
  oled.begin();
  oled.clear(PAGE);
  oled.clear(ALL);
  oled.drawBitmap(logo_bmp);//call the drawBitmap function and pass it the array from above
  oled.setFontType(0);
  oled.setCursor(0, 36);
  oled.print(" IoTbundle");
  oled.display();

  //  pzem.resetEnergy(); //reset energy

  dht1.begin();
  dht2.begin();

  Blynk.begin(auth, ssid, pass);

  // Set interrupt
  attachInterrupt(digitalPinToInterrupt(D5), handleInterrupt, RISING);
}

ICACHE_RAM_ATTR void handleInterrupt() {
  T_prevDetect = T_Detect;
  T_Detect = micros();
  T_diff += (T_Detect - T_prevDetect); //?????????????????????????????? 1 ?????????
  N++;
}

void loop() {
  Blynk.run();
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) { //run every 1 second
    previousMillis = currentMillis;
    displayValue(); //update OLED

    //------DHT ------
    float h1 = dht1.readHumidity();
    float t1 = dht1.readTemperature();
    Serial.print(F("Humidity1: "));
    Serial.print(h1);
    Serial.print(F("%  Temperature1: "));
    Serial.print(t1);
    Serial.println(F("??C "));

    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();
    Serial.print(F("Humidity2: "));
    Serial.print(h2);
    Serial.print(F("%  Temperature2: "));
    Serial.print(t2);
    Serial.println(F("??C "));
    Serial.println("");


    //------sent data to blynk------
    Blynk.virtualWrite(V1, voltage);
    Blynk.virtualWrite(V2, current);
    Blynk.virtualWrite(V3, power);
    Blynk.virtualWrite(V4, energy);
    Blynk.virtualWrite(V5, frequency);
    Blynk.virtualWrite(V6, pf);
    //------sent DHT to blynk------
    Blynk.virtualWrite(V9, h1);
    Blynk.virtualWrite(V10, t1);
    Blynk.virtualWrite(V11, h2);
    Blynk.virtualWrite(V12, t2);
  }

  unsigned long currentMillisRPM = millis();
  if (currentMillisRPM - previousMillisRPM >= 500) {

    if (N) {
      previousMillisRPM = currentMillisRPM;
      T_diff = T_diff / N;  //average of time[uSec] per round[N]
      RPM = 60000000 / T_diff;  //1 minute = 60 microsecond

      Serial.print("RPM: "); Serial.println(RPM);
      Blynk.virtualWrite(V8, RPM); //sent to blynk

      //      oled.clear(PAGE);oled.setFontType(1);oled.setCursor(0, 0);oled.print(RPM);oled.display();

      //clear
      T_diff = 0;
      N = 0;
    }
    else if (currentMillisRPM - previousMillisRPM >= 2000) {
      previousMillisRPM = currentMillisRPM;
      RPM = 0;

      //      oled.clear(PAGE);oled.setFontType(1);oled.setCursor(0, 0);oled.print(RPM); oled.display();
    }
  }
}

void displayValue() {
  //------read data------
  voltage = pzem.voltage();
  current = pzem.current();
  power = pzem.power();
  energy = pzem.energy();
  frequency = pzem.frequency();
  pf = pzem.pf();

  //------Update OLED display------
  oled.clear(PAGE);
  oled.setFontType(0);

  //display voltage
  oled.setCursor(3, 0);  oled.print(voltage, 1); oled.setCursor(46, 0); oled.println("V");

  //display current
  if (current < 10) oled.setCursor(9, 12);
  else oled.setCursor(3, 12);
  oled.print(current, 2); oled.setCursor(46, 12); oled.println("A");

  //display power
  if (power < 10) oled.setCursor(26, 24);
  else if (power < 100)  oled.setCursor(20, 24);
  else if (power < 1000)  oled.setCursor(14, 24);
  else if (power < 10000)  oled.setCursor(8, 24);
  else  oled.setCursor(2, 24);
  oled.print(power, 0); oled.setCursor(46, 24); oled.println("W");

  //display energy
  oled.setCursor(3, 36);
  if (energy < 10) oled.print(energy, 3);
  else if (energy < 100) oled.print(energy, 2);
  else if (energy < 1000) oled.print(energy, 1);
  else {
    oled.setCursor(8, 36);
    oled.print(energy, 0);
  } oled.setCursor(46, 36); oled.println("kWh");

  //on error
  if (isnan(voltage)) {
    oled.clear(PAGE);
    oled.setCursor(0, 0);
    oled.printf("Please\n\nConnect\n\nPZEM004T");
  }
  oled.display();

  //------Serial display------
  Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  Serial.print("Current: "); Serial.print(current); Serial.println("A");
  Serial.print("Power: "); Serial.print(power); Serial.println("W");
  Serial.print("Energy: "); Serial.print(energy, 3); Serial.println("kWh");
  Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
  Serial.print("PF: "); Serial.println(pf);
}
