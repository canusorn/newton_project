/*
   โหลด Library "HX711" จาก Library manager

   OUTPUT Active LOW
   function2 เช็คน้ำหนัก ถ้านอ้ยกว่าให้ทำงาน หรือหยุดทำงาน OUTPUT ที่ D6
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

*/

#include "HX711.h"

HX711 scale;

uint8_t dataPin = D3;
uint8_t clockPin = D4;

float w1, w2, previous = 0;

float weight, delta; // น้ำหนัก , ความเปลี่ยนแปลงน้ำหนัก       

// function2 ตัดต่อการทำงาน
float weight_cutoff = 100;                               // #3 ช่วงที่ให้ตัดการทำงาน [gram]
float weight_error = 5;                                  // #4 ช่วง +/- ช่วงตัดการทำงาน [gram]  Ex. ทำงานที่ 100-5=95 หยุดที่ 100+5=105

unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(115200);

  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  digitalWrite(D6, HIGH);
  pinMode(D6, OUTPUT);

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
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;

    scalecheck();  //วัดน้ำหนัก

    // function2
    if (weight < weight_cutoff - weight_error) {
      digitalWrite(D6, HIGH);  //ภ้าให้นอ้ยกว่า cutoff แล้วหยุดทำงาน               // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
      //      digitalWrite(D6,LOW);    //ภ้าให้นอ้ยกว่า cutoff แล้วทำงาน                // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
    }
    else if (weight > weight_cutoff + weight_error) {
      digitalWrite(D6, LOW);    //ภ้าให้นอ้ยกว่า cutoff แล้วหยุดทำงาน              // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
      //      digitalWrite(D6,HIGH);    //ภ้าให้นอ้ยกว่า cutoff แล้วทำงาน               // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
    }
    Serial.println("\tRelay :" + String(!digitalRead(D6)));
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
