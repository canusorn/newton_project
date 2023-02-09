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
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"

#define BLYNK_PRINT Serial

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
  T_diff += (T_Detect - T_prevDetect); //คำนวณค่าใน 1 รอบ
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
    Serial.println(F("°C "));

    float h2 = dht2.readHumidity();
    float t2 = dht2.readTemperature();
    Serial.print(F("Humidity2: "));
    Serial.print(h2);
    Serial.print(F("%  Temperature2: "));
    Serial.print(t2);
    Serial.println(F("°C "));
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

  //------Serial display------
  Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
  Serial.print("Current: "); Serial.print(current); Serial.println("A");
  Serial.print("Power: "); Serial.print(power); Serial.println("W");
  Serial.print("Energy: "); Serial.print(energy, 3); Serial.println("kWh");
  Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
  Serial.print("PF: "); Serial.println(pf);
}
