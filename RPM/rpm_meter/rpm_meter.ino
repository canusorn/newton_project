#include <Wire.h>
#include <SFE_MicroOLED.h>

#define PIN_RESET -1
#define DC_JUMPER 0

MicroOLED oled(PIN_RESET, DC_JUMPER);

volatile byte interruptCounter = 0;
int N;
uint32_t RPM;
uint64_t T_Detect, T_prevDetect, T_diff;
bool updateRPM;
unsigned long previousMillisRPM;

void setup() {

  Serial.begin(115200);
  pinMode(D5, INPUT);
  Wire.begin();

  oled.begin();
  oled.clear(PAGE);
  oled.clear(ALL);
  oled.display();

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

  unsigned long currentMillisRPM = millis();
  if (currentMillisRPM - previousMillisRPM >= 500) {

    if (N) {
      previousMillisRPM = currentMillisRPM;
      T_diff = T_diff / N;  //average of time[uSec] per round[N]
      RPM = 60000000 / T_diff;  //1 minute = 60 microsecond

      Serial.print("RPM: "); Serial.println(RPM);

      oled.clear(PAGE); oled.setFontType(1); oled.setCursor(0, 0); oled.print(RPM); oled.display();

      //clear
      T_diff = 0;
      N = 0;
    }
    else if (currentMillisRPM - previousMillisRPM >= 2000) {
      previousMillisRPM = currentMillisRPM;
      RPM = 0;

      oled.clear(PAGE); oled.setFontType(1); oled.setCursor(0, 0); oled.print(RPM); oled.display();
    }
  }
}
