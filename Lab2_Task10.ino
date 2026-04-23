#include <Arduino_APDS9960.h>
#include <Arduino_BMI270_BMM150.h>
#include <PDM.h>

// Thresholds 
const float SOUND_THRESHOLD  = 1200.0;
const int   LIGHT_THRESHOLD  = 50;
const float MOTION_THRESHOLD = 20.0;
const int   PROX_THRESHOLD   = 80;

// Microphone 
short sampleBuffer[256];
volatile float micLevel = 0.0;

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);

  int samples = bytesAvailable / 2;
  long sum = 0;

  for (int i = 0; i < samples; i++) {
    sum += abs(sampleBuffer[i]);
  }

  if (samples > 0) {
    micLevel = (float)sum / samples;
  }
}

// Last sensor values
int clearValue = 0;
int proxValue = 0;
float motionValue = 0.0;

void setup() {
  Serial.begin(115200);
  delay(1500);

  if (!APDS.begin()) {
    Serial.println("Failed to initialize APDS9960 sensor.");
    while (1);
  }

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU.");
    while (1);
  }

  PDM.onReceive(onPDMdata);
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to initialize microphone.");
    while (1);
  }

  Serial.println("Workspace situation classifier started");
}

void loop() {
  // Read light
  if (APDS.colorAvailable()) {
    int r, g, b, c;
    APDS.readColor(r, g, b, c);
    clearValue = c;
  }

  // Read proximity 
  if (APDS.proximityAvailable()) {
    proxValue = APDS.readProximity();
  }

  // Read motion from gyroscope 
  if (IMU.gyroscopeAvailable()) {
    float x, y, z;
    IMU.readGyroscope(x, y, z);
    motionValue = sqrt(x * x + y * y + z * z);
  }

  // Copy microphone value 
  float currentMic;
  noInterrupts();
  currentMic = micLevel;
  interrupts();

  // Binary flags 
  int sound  = (currentMic > SOUND_THRESHOLD) ? 1 : 0;
  int dark   = (clearValue < LIGHT_THRESHOLD) ? 1 : 0;
  int moving = (motionValue > MOTION_THRESHOLD) ? 1 : 0;
  int near   = (proxValue > PROX_THRESHOLD) ? 1 : 0;

  // Rule-based classification
  String state;

  if (sound == 0 && dark == 0 && moving == 0 && near == 0) {
    state = "QUIET_BRIGHT_STEADY_FAR";
  } 
  else if (sound == 1 && dark == 0 && moving == 0 && near == 0) {
    state = "NOISY_BRIGHT_STEADY_FAR";
  } 
  else if (sound == 0 && dark == 1 && moving == 0 && near == 1) {
    state = "QUIET_DARK_STEADY_NEAR";
  } 
  else if (sound == 1 && dark == 0 && moving == 1 && near == 1) {
    state = "NOISY_BRIGHT_MOVING_NEAR";
  } 
  else {
    // Fallback so the program always prints one of the four required labels
    if (dark == 1 && near == 1 && moving == 0) {
      state = "QUIET_DARK_STEADY_NEAR";
    } 
    else if (moving == 1 && near == 1) {
      state = "NOISY_BRIGHT_MOVING_NEAR";
    } 
    else if (sound == 1) {
      state = "NOISY_BRIGHT_STEADY_FAR";
    } 
    else {
      state = "QUIET_BRIGHT_STEADY_FAR";
    }
  }

  // Required Serial Monitor format
  Serial.print("raw,mic=");
  Serial.print(currentMic, 1);
  Serial.print(",clear=");
  Serial.print(clearValue);
  Serial.print(",motion=");
  Serial.print(motionValue, 2);
  Serial.print(",prox=");
  Serial.println(proxValue);

  Serial.print("flags,sound=");
  Serial.print(sound);
  Serial.print(",dark=");
  Serial.print(dark);
  Serial.print(",moving=");
  Serial.print(moving);
  Serial.print(",near=");
  Serial.println(near);

  Serial.print("state,");
  Serial.println(state);

  delay(300);
}