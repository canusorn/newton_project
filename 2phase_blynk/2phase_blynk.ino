/*
  -PZEM004T-
  5V - 5V
  GND - GND
  D3 - TX
  D4 - RX

*/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

/* Fill-in your Template ID (only if using Blynk.Cloud) */
#define BLYNK_TEMPLATE_ID "YourTemplateID"

// alarm pin
#define ALARM_PIN_1 D5
#define ALARM_CURR_1 20
#define ALARM_PIN_2 D6
#define ALARM_CURR_2 20

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <PZEM004Tv30.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "YourAuthToken";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "YourNetworkName";
char pass[] = "YourPassword";

#define PIN_RESET -1
#define DC_JUMPER 0

#define PHASE 2
MicroOLED oled(PIN_RESET, DC_JUMPER); // Example I2C declaration, uncomment if using I2C

unsigned long previousMillis = 0;
uint8_t phase_display;

uint8_t logo_bmp[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xC0, 0xF0, 0xE0, 0x78, 0x38, 0x78, 0x3C, 0x1C, 0x3C, 0x1C, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1C, 0x3C, 0x1C, 0x3C, 0x78, 0x38, 0xF0, 0xE0, 0xF0, 0xC0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x01, 0x00, 0x00, 0xF0, 0xF8, 0x70, 0x3C, 0x3C, 0x1C, 0x1E, 0x1E, 0x0E, 0x0E, 0x0E, 0x0F, 0x0F, 0x0E, 0x0E, 0x1E, 0x1E, 0x1E, 0x3C, 0x1C, 0x7C, 0x70, 0xF0, 0x70, 0x20, 0x01, 0x01, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1C, 0x3E, 0x1E, 0x0F, 0x0F, 0x07, 0x87, 0x87, 0x07, 0x0F, 0x0F, 0x1E, 0x3E, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x1F, 0x1F, 0x3F, 0x3F, 0x1F, 0x1F, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

void setup()
{
  // Debug console
  Serial.begin(115200);
  Wire.begin();

  digitalWrite(ALARM_PIN_1, LOW);
  digitalWrite(ALARM_PIN_2, LOW);
  pinMode(ALARM_PIN_1, OUTPUT);
  pinMode(ALARM_PIN_2, OUTPUT);

  //------Display LOGO at start------
  oled.begin();
  oled.clear(PAGE);
  oled.clear(ALL);
  oled.drawBitmap(logo_bmp); // call the drawBitmap function and pass it the array from above
  oled.setFontType(0);
  oled.setCursor(0, 36);
  oled.print(" IoTbundle");
  oled.display();

  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000)
  { // run every 1 second
    previousMillis = currentMillis;

    phase_display++;
    if (phase_display >= 2)
      phase_display = 0;

    displayValue(); // update OLED
  }
}
void displayValue()
{

  PZEM004Tv30 pzems[] = {PZEM004Tv30(D3, D4, 0x01), PZEM004Tv30(D3, D4, 0x02)}; // array of pzem 2 phase
  float voltage[PHASE], current[PHASE], power[PHASE], energy[PHASE], frequency[PHASE], pf[PHASE];

  for (int i = 0; i < PHASE; i++)
  {
    //------read data------
    voltage[i] = pzems[i].voltage();
    if (!isnan(voltage[i]))
    { // ???????????????????????????????????????
      current[i] = pzems[i].current();
      power[i] = pzems[i].power();
      energy[i] = pzems[i].energy();
      frequency[i] = pzems[i].frequency();
      pf[i] = pzems[i].pf();
    }
    else
    { // ??????????????????????????????????????????????????????????????????????????? NAN(not a number)
      current[i] = NAN;
      power[i] = NAN;
      energy[i] = NAN;
      frequency[i] = NAN;
      pf[i] = NAN;
    }

    //------Serial display------
    if (!isnan(voltage[i]))
    {
      Serial.print(F("PZEM "));
      Serial.print(i);
      Serial.print(F(" - Address:"));
      Serial.println(pzems[i].getAddress(), HEX);
      Serial.println(F("==================="));
      Serial.print(F("Voltage: "));
      Serial.print(voltage[i]);
      Serial.println("V");
      Serial.print(F("Current: "));
      Serial.print(current[i]);
      Serial.println(F("A"));
      Serial.print(F("Power: "));
      Serial.print(power[i]);
      Serial.println(F("W"));
      Serial.print(F("Energy: "));
      Serial.print(energy[i], 3);
      Serial.println(F("kWh"));
      Serial.print(F("Frequency: "));
      Serial.print(frequency[i], 1);
      Serial.println(F("Hz"));
      Serial.print(F("PF: "));
      Serial.println(pf[i]);
      Serial.println(F("-------------------"));
      Serial.println();
    }
    else
    {
      Serial.println("No sensor detect");
    }
  }

  // blynk update
  Blynk.virtualWrite(V0, voltage[0]);
  Blynk.virtualWrite(V1, current[0]);
  Blynk.virtualWrite(V2, power[0]);
  Blynk.virtualWrite(V3, energy[0]);
  Blynk.virtualWrite(V4, frequency[0]);
  Blynk.virtualWrite(V5, pf[0]);
  Blynk.virtualWrite(V6, voltage[1]);
  Blynk.virtualWrite(V7, current[1]);
  Blynk.virtualWrite(V8, power[1]);
  Blynk.virtualWrite(V9, energy[1]);
  Blynk.virtualWrite(V10, frequency[1]);
  Blynk.virtualWrite(V11, pf[1]);

  // Alarm current phase 1
  if (current[0] > ALARM_CURR_1)
  {
    digitalWrite(ALARM_PIN_1, HIGH);
  }
  else
  {
    digitalWrite(ALARM_PIN_1, LOW);
  }

  // Alarm current phase 2
  if (current[1] > ALARM_CURR_2)
  {
    digitalWrite(ALARM_PIN_2, HIGH);
  }
  else
  {
    digitalWrite(ALARM_PIN_2, LOW);
  }

  //------Update OLED display------
  oled.clear(PAGE);
  oled.setFontType(0);

  oled.setCursor(0, 0);
  oled.println("Phase " + String(phase_display + 1));

  // display voltage
  oled.setCursor(3, 12);
  oled.print(voltage[phase_display], 1);
  oled.setCursor(42, 12);
  oled.println("V");

  // display current
  if (current[phase_display] < 10)
    oled.setCursor(9, 21);
  else
    oled.setCursor(3, 21);
  oled.print(current[phase_display], 2);
  oled.setCursor(42, 21);
  oled.println("A");

  // display power
  if (power[phase_display] < 10)
    oled.setCursor(26, 30);
  else if (power[phase_display] < 100)
    oled.setCursor(20, 30);
  else if (power[phase_display] < 1000)
    oled.setCursor(14, 30);
  else if (power[phase_display] < 10000)
    oled.setCursor(8, 30);
  else
    oled.setCursor(2, 30);
  oled.print(power[phase_display], 0);
  oled.setCursor(42, 30);
  oled.println("W");

  // display energy
  oled.setCursor(3, 40);
  if (energy[phase_display] < 10)
    oled.print(energy[phase_display], 3);
  else if (energy[phase_display] < 100)
    oled.print(energy[phase_display], 2);
  else if (energy[phase_display] < 1000)
    oled.print(energy[phase_display], 1);
  else
  {
    oled.setCursor(8, 40);
    oled.print(energy[phase_display], 0);
  }
  oled.setCursor(42, 40);
  oled.println("kWh");

  // on error
  if (isnan(voltage[phase_display]))
  {
    oled.clear(PAGE);
    oled.setCursor(0, 0);
    oled.println("Phase " + String(phase_display + 1));
    oled.printf("\nno sensor\ndetect!");
  }

  oled.display();
}
