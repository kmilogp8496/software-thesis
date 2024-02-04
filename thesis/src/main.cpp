#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
  Serial.begin(230400);
  Serial.println("Serial connected");
}

void loop() {
  delay(1000);
  Serial.println("Serial sendind data" + String(millis()));
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}