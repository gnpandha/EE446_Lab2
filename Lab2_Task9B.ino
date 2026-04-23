#include <Arduino_APDS9960.h>

void setup() {
  Serial.begin(115200);
  delay(3000);

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
  }

  Serial.println("Gesture test started");
}

void loop() {
  if (APDS.gestureAvailable()) {
    int gesture = APDS.readGesture();

    if (gesture == GESTURE_UP) {
      Serial.println("UP");
    } else if (gesture == GESTURE_DOWN) {
      Serial.println("DOWN");
    } else if (gesture == GESTURE_LEFT) {
      Serial.println("LEFT");
    } else if (gesture == GESTURE_RIGHT) {
      Serial.println("RIGHT");
    }
  }
}