/*
   โหลด Library "HX711" จาก Library manager
   โหลด Library "Blynk" จาก Library manager

   OUTPUT Active LOW
   function1 เช็คการเปลี่ยนแปลงน้ำหนัก alarm ที่ D5
   -----------------------------------------------------------
   Schematic
   Wemos       HX117      Relay
   5V          VCC         VCC
   G           GND         GND
   D3          DT          
   D4          SCK         
   D5                      IN1
   D6                      IN2
   -----------------------------------------------------------
   Blynk Pin

   V0 - แสดงค่าน้ำหนัก
   V1 - แสดงค่าการเปลี่ยนแปลงน้ำหนัก(delta)

   V2 - แสดงค่า function1 Alarm
   V3 - รับค่าช่วง +/- น้ำหนักเปลี่ยนแปลง [gram] (delta_weight)    #1
   V4 - รับค่าเวลาที่น้ำหนักไม่เปลี่ยนแล้วให้ alarm [second]           #2

*/

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include "HX711.h"

HX711 scale;

uint8_t dataPin = D3;
uint8_t clockPin = D4;

float w1, w2, previous = 0;

char auth[] = "token";        // #0  Token Blynk
char ssid[] = "ssid";                               // #0  ชื่อไวไฟ
char pass[] = "pass";                               // #0  รหัสไวไฟ

// function1 เช็คค่าเปลี่ยนแปลงน้ำหนัก
float weight, delta; // น้ำหนัก , ความเปลี่ยนแปลงน้ำหนัก
float delta_weight = 100;                                // #1 ช่วง +/- น้ำหนักเปลี่ยนแปลง [gram] ที่ต้องการให้แจ้งเตือน    Ex. หนัก 500g เริ่มแจ้งเตือนเมื่อ ค่าอยู่ระหว่าง 500+/-100 -> 400 ถึง 600
uint16_t t, t_noactive = 60;                             // #2 เวลาที่น้ำหนักไม่เปลี่ยนแล้วให้ alarm [second]

unsigned long previousMillis = 0;

BLYNK_WRITE(V3)
{
  delta_weight = param.asInt();
}

BLYNK_WRITE(V4)
{
  t_noactive = param.asInt();
}

void setup()
{
  Serial.begin(115200);

  //blnyk start
  Blynk.begin(auth, ssid, pass);
  Blynk.virtualWrite(V3, delta_weight);
  Blynk.virtualWrite(V4, t_noactive);

  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  digitalWrite(D5, HIGH);
  pinMode(D5, OUTPUT);

  scale.begin(dataPin, clockPin);

  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));

  // loadcell factor 5 KG
  scale.set_scale(394.0745);           // ปรับคาลิเบรท (ค่าเดิม 420.0983)
  scale.tare();                        // set zero

  Serial.print("UNITS: ");
  Serial.println(scale.get_units(10));
}

void loop()
{
  Blynk.run();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    scalecheck();  //วัดน้ำหนัก
    blynkupdate();  //Blynk update

    // function1
    if (abs(delta) <= delta_weight) {    // ถ้าการเปลี่ยนแปลงน้อยกว่า weight_delta ให้จับเวลา
      if (t < t_noactive)t++;
    }
    else  t = 0;    // ถ้าการเปลี่ยนแปลงมากกว่า weight_delta ให้รีเซ้ตการจับเวลา

    if (t >= t_noactive) digitalWrite(D5, LOW);    // ถ้าน้ำหนักไม่เปลี่ยนแปลงครบเวลา ให้ Relay ทำงาน
    else digitalWrite(D5, HIGH);
    Serial.print("\tTimetoAlarm: " + String(t));
    Serial.println("\tRelay1: " + String(!digitalRead(D5)));
  }

}

void scalecheck() {
  // read until stable
  w1 = scale.get_units(10);
  //  delay(100);
  w2 = scale.get_units();
  while (abs(w1 - w2) > 10)
  {
    w1 = w2;
    w2 = scale.get_units();
    //     delay(100);
  }

  Serial.print("UNITS: ");
  Serial.print(w1);
  weight = w1;
  if (w1 == 0)
  {
    Serial.println();
  }
  else
  {
    Serial.print("\tDELTA: ");
    Serial.print(w1 - previous);
    delta = w1 - previous;
    previous = w1;
  }
  //  delay(100);
}

void blynkupdate() {
  Blynk.virtualWrite(V0, weight);
  Blynk.virtualWrite(V1, delta);
  Blynk.virtualWrite(V2, !digitalRead(D5));
}
