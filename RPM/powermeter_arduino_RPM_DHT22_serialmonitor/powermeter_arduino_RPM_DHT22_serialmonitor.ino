/*
   -PZEM004T-
   5V - 5V
   GND - GND
   3 - TX
   4 - RX

   RPM-2

   DHT22(1) - 6
   DHT22(2) - 7
*/
#include <PZEM004Tv30.h>
#include "DHT.h"


PZEM004Tv30 pzem(3, 4); //rx,tx pin  Software serial
//PZEM004Tv30 pzem(&Serial);    //Hardware serial
DHT dht1(6, DHT22);
DHT dht2(7, DHT22);

unsigned long previousMillis = 0;
float voltage, current, power, energy, frequency, pf;

int N;
uint32_t RPM;
uint64_t T_Detect, T_prevDetect, T_diff;
bool updateRPM;
unsigned long previousMillisRPM;

void setup() {
  Serial.begin(115200);
  pinMode(2, INPUT);


  //  pzem.resetEnergy(); //reset energy

  dht1.begin();
  dht2.begin();

  Serial.println("start");
  // Set interrupt
  attachInterrupt(digitalPinToInterrupt(2), handleInterrupt, RISING);
}

void handleInterrupt() {
  T_prevDetect = T_Detect;
  T_Detect = micros();
  T_diff += (T_Detect - T_prevDetect); //คำนวณเวลาใน 1 รอบ
  N++;
}

void loop() {
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
    //
    //    float h2 = dht2.readHumidity();
    //    float t2 = dht2.readTemperature();
    //    Serial.print(F("Humidity2: "));
    //    Serial.print(h2);
    //    Serial.print(F("%  Temperature2: "));
    //    Serial.print(t2);
    //    Serial.println(F("°C "));
    //    Serial.println("");

  }

  unsigned long currentMillisRPM = millis();
  if (currentMillisRPM - previousMillisRPM >= 1000) {

    if (N) {
      previousMillisRPM = currentMillisRPM;
      T_diff = T_diff / N;  //average of time[uSec] per round[N]
      RPM = 60000000 / T_diff;  //1 minute = 60 microsecond

      Serial.print("RPM: "); Serial.println(RPM);
      Serial.println("");

      //clear
      T_diff = 0;
      N = 0;
    }
    else if (currentMillisRPM - previousMillisRPM >= 2000) {
      previousMillisRPM = currentMillisRPM;
      RPM = 0;
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
