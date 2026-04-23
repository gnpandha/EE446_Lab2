#include <Arduino_HS300x.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!HS300x.begin()) {
    Serial.println("Failed to initialize humidity/temperature sensor.");
    while (1);
  }

  Serial.println("Humidity and temperature test started");
}

void loop() {
  float temperature = HS300x.readTemperature();
  float humidity = HS300x.readHumidity();

  Serial.print("Temperature (C): ");
  Serial.print(temperature, 2);
  Serial.print(" | Humidity (%): ");
  Serial.println(humidity, 2);

  delay(1000);
}