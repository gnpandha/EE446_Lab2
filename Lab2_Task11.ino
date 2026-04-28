#include <Arduino_HS300x.h>
#include <Arduino_BMI270_BMM150.h>
#include <Arduino_APDS9960.h>

const float HUMID_THRESHOLD   = 4.0;
const float TEMP_THRESHOLD    = 1.0;
const float MAG_THRESHOLD     = 120.0;
const int   LIGHT_THRESHOLD   = 30;
const int   COLOR_THRESHOLD   = 30;
const unsigned long COOLDOWN_MS = 2000;

float baseRH, baseTemp, baseMag;
int   baseR, baseG, baseB, baseClear;
String lastEvent = "";
unsigned long lastEventTime = 0;

void setup() {
  Serial.begin(115200);
  
  // Blink LED so you know board is alive
  pinMode(LED_BUILTIN, OUTPUT);
  for (int i = 0; i < 6; i++) {
    digitalWrite(LED_BUILTIN, i % 2);
    delay(300);
  }

  delay(1500);
  Serial.println("Task 11 starting...");

  if (!HS300x.begin()) {
    Serial.println("ERROR: HS300x failed");
    while (1);
  }
  Serial.println("HS300x OK");

  if (!IMU.begin()) {
    Serial.println("ERROR: IMU failed");
    while (1);
  }
  Serial.println("IMU OK");

  if (!APDS.begin()) {
    Serial.println("ERROR: APDS9960 failed");
    while (1);
  }
  Serial.println("APDS9960 OK");

  delay(1000);

  // Baseline: humidity & temperature
  baseRH   = HS300x.readHumidity();
  baseTemp = HS300x.readTemperature();

  // Baseline: magnetometer (average 5 readings)
  baseMag = 0;
  int magCount = 0;
  for (int i = 0; i < 5; i++) {
    delay(100);
    if (IMU.magneticFieldAvailable()) {
      float mx, my, mz;
      IMU.readMagneticField(mx, my, mz);
      baseMag += sqrt(mx*mx + my*my + mz*mz);
      magCount++;
    }
  }
  if (magCount > 0) baseMag /= magCount;

  // Baseline: color
  baseR = 0; baseG = 0; baseB = 0; baseClear = 0;
  for (int i = 0; i < 10; i++) {
    delay(100);
    if (APDS.colorAvailable()) {
      APDS.readColor(baseR, baseG, baseB, baseClear);
      break;
    }
  }

  Serial.print("Baseline — rh=");
  Serial.print(baseRH, 2);
  Serial.print(" temp=");
  Serial.print(baseTemp, 2);
  Serial.print(" mag=");
  Serial.print(baseMag, 2);
  Serial.print(" clear=");
  Serial.println(baseClear);
  Serial.println("--- monitoring started ---");
}

void loop() {
  float rh   = HS300x.readHumidity();
  float temp = HS300x.readTemperature();

  float mag = baseMag;
  if (IMU.magneticFieldAvailable()) {
    float mx, my, mz;
    IMU.readMagneticField(mx, my, mz);
    mag = sqrt(mx*mx + my*my + mz*mz);
  }

  int r = baseR, g = baseG, b = baseB, clearVal = baseClear;
  if (APDS.colorAvailable()) {
    APDS.readColor(r, g, b, clearVal);
  }

  int humid_jump            = (abs(rh - baseRH)  > HUMID_THRESHOLD) ? 1 : 0;
  int temp_rise             = (temp - baseTemp    > TEMP_THRESHOLD)  ? 1 : 0;
  int mag_shift             = (abs(mag - baseMag) > MAG_THRESHOLD)   ? 1 : 0;
  int lightDiff             = abs(clearVal - baseClear);
  int colorDiff             = abs(r-baseR) + abs(g-baseG) + abs(b-baseB);
  int light_or_color_change = (lightDiff > LIGHT_THRESHOLD || colorDiff > COLOR_THRESHOLD) ? 1 : 0;

  String eventLabel = "BASELINE_NORMAL";
  if (mag_shift) {
    eventLabel = "MAGNETIC_DISTURBANCE_EVENT";
  } else if (humid_jump || temp_rise) {
    eventLabel = "BREATH_OR_WARM_AIR_EVENT";
  } else if (light_or_color_change) {
    eventLabel = "LIGHT_OR_COLOR_CHANGE_EVENT";
  }

  unsigned long now = millis();
  if (eventLabel != lastEvent || (now - lastEventTime) >= COOLDOWN_MS) {
    lastEvent     = eventLabel;
    lastEventTime = now;
  }

  Serial.print("raw,rh=");
  Serial.print(rh, 2);
  Serial.print(",temp=");
  Serial.print(temp, 2);
  Serial.print(",mag=");
  Serial.print(mag, 2);
  Serial.print(",r=");
  Serial.print(r);
  Serial.print(",g=");
  Serial.print(g);
  Serial.print(",b=");
  Serial.print(b);
  Serial.print(",clear=");
  Serial.println(clearVal);

  Serial.print("flags,humid_jump=");
  Serial.print(humid_jump);
  Serial.print(",temp_rise=");
  Serial.print(temp_rise);
  Serial.print(",mag_shift=");
  Serial.print(mag_shift);
  Serial.print(",light_or_color_change=");
  Serial.println(light_or_color_change);

  Serial.print("event,");
  Serial.println(eventLabel);

  delay(500);
}