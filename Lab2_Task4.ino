void setup () {
  Serial.begin (115200);
  delay (1500);
}

void loop() {
  static int x = 0;
  int y = (x % 20);
  Serial.println (y);
  x++; 
  delay (100);
}
