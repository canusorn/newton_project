// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor

#include "DHT.h"

DHT dht1(D6, DHT22);
DHT dht2(D7, DHT22);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht1.begin();
  dht2.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  float h1 = dht1.readHumidity();
  float h2 = dht2.readHumidity();
  float t1 = dht1.readTemperature();
  float t2 = dht2.readTemperature();


  Serial.print(F("Humidity: "));
  Serial.print(h1);
  Serial.print(F("%  Temperature: "));
  Serial.print(t1);
  Serial.println(F("°C "));

  Serial.print(F("Humidity: "));
  Serial.print(h2);
  Serial.print(F("%  Temperature: "));
  Serial.print(t2);
  Serial.println(F("°C "));
  Serial.println("");
}
