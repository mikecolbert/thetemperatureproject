const int redLED = 13;           // Red onboard LED

void setup() {
  pinMode(redLED, OUTPUT);
}

void loop() {
  digitalWrite(redLED, HIGH);
  delay(3000);

  digitalWrite(redLED, LOW);
  delay(500);
}