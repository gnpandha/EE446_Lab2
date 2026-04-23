#include <Arduino_APDS9960.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
  }

  Serial.println("Ambient light and color test started");
  Serial.println("r,g,b,clear");
}

void loop() {
  int r, g, b, c;

  if (APDS.colorAvailable()) {
    APDS.readColor(r, g, b, c);

    Serial.print(r);
    Serial.print(",");
    Serial.print(g);
    Serial.print(",");
    Serial.print(b);
    Serial.print(",");
    Serial.println(c);
  }

  delay(200);
}