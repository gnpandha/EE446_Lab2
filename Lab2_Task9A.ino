#include <Arduino_APDS9960.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
  }

  Serial.println("Proximity test started");
}

void loop() {
  if (APDS.proximityAvailable()) {
    int proximity = APDS.readProximity();

    Serial.print("Proximity: ");
    Serial.println(proximity);
  }

  delay(100);
}