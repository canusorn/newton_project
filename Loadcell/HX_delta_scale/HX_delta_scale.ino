/*
 * โหลด Library "HX711" จาก Library manager
 * 
 * OUTPUT Active LOW
 * function1 เช็คการเปลี่ยนแปลงน้ำหนัก alarm ที่ D5
 * function2 เช็คน้ำหนัก ถ้านอ้ยกว่าให้ทำงาน หรือหยุดทำงาน OUTPUT ที่ D6
 */

#include "HX711.h"

HX711 scale;

uint8_t dataPin = D3;
uint8_t clockPin = D4;

float w1, w2, previous = 0;

// function1 เช็คค่าเปลี่ยนแปลงน้ำหนัก
float weight, delta; // น้ำหนัก , ความเปลี่ยนแปลงน้ำหนัก
float delta_weight = 100;                                // #1 ช่วง +/- น้ำหนักเปลี่ยนแปลง [gram] ที่ต้องการให้แจ้งเตือน    Ex. หนัก 500g เริ่มแจ้งเตือนเมื่อ ค่าอยู่ระหว่าง 500+/-100 -> 400 ถึง 600
uint16_t t, t_noactive = 60;                             // #2 เวลาที่น้ำหนักไม่เปลี่ยน [second]

// function2 ตัดต่อการทำงาน
float weight_cutoff = 100;                               // #3 ช่วงที่ให้ตัดการทำงาน [gram]
float weight_error = 10;                                  // #4 ช่วง +/- ช่วงตัดการทำงาน [gram]  Ex. ทำงานที่ 100-5=95 หยุดที่ 100+5=105

unsigned long previousMillis = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println(__FILE__);
  Serial.print("LIBRARY VERSION: ");
  Serial.println(HX711_LIB_VERSION);
  Serial.println();

  digitalWrite(D5, HIGH);
  digitalWrite(D6, HIGH);
  pinMode(D5, OUTPUT);
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

    // function1
    if (abs(delta) <= delta_weight) {    // ถ้าการเปลี่ยนแปลงน้อยกว่า weight_delta ให้จับเวลา
      if (t < t_noactive)t++;
    }
    else  t = 0;    // ถ้าการเปลี่ยนแปลงมากกว่า weight_delta ให้รีเซ้ตการจับเวลา

    if (t >= t_noactive) digitalWrite(D5, LOW);    // ถ้าน้ำหนักไม่เปลี่ยนแปลงครบเวลา ให้ Relay ทำงาน
    else digitalWrite(D5, HIGH);
    Serial.print("\tTimetoAlarm: " + String(t));

    // function2
    if(weight < weight_cutoff - weight_error){
      digitalWrite(D6,HIGH);   //ภ้าให้นอ้ยกว่า cutoff แล้วหยุดทำงาน               // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
//      digitalWrite(D6,LOW);    //ภ้าให้นอ้ยกว่า cutoff แล้วทำงาน                // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
    }
    else if(weight > weight_cutoff + weight_error){
      digitalWrite(D6,LOW);     //ภ้าให้นอ้ยกว่า cutoff แล้วหยุดทำงาน              // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
//      digitalWrite(D6,HIGH);    //ภ้าให้นอ้ยกว่า cutoff แล้วทำงาน               // #5 เลือกเมื่อน้ำหนักน้อยกว่า ให้ทำงาน หรือหยุดทำงาน
    }
    Serial.println("\tRelay1: " + String(!digitalRead(D5)) + "\tRelay2 :" + String(!digitalRead(D6)));
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
