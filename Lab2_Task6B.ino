#include <Arduino_BMI270_BMM150.h>

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU.");
    while (1);
  }

  Serial.println("Gyroscope test started");
  Serial.println("gx,gy,gz");
}

void loop() {
  float x, y, z;

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);

    Serial.print(x, 3);
    Serial.print(",");
    Serial.print(y, 3);
    Serial.print(",");
    Serial.println(z, 3);
  }

  delay(100);
}