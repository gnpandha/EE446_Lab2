#include <Arduino_LPS22HB.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!BARO.begin()) {
    Serial.println("Failed to initialize barometric pressure sensor.");
    while (1);
  }

  Serial.println("Barometric pressure test started");
}

void loop() {
  float pressure = BARO.readPressure();
  float temperature = BARO.readTemperature();

  Serial.print("Pressure (kPa): ");
  Serial.print(pressure, 3);
  Serial.print(" | Temperature (C): ");
  Serial.println(temperature, 2);

  delay(1000);
}