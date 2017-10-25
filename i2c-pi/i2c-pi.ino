#include <Wire.h>

#define SLAVE_ADDRESS 0x04

int number = 0;

void setup() {
  pinMode(8, OUTPUT);
  digitalWrite(8, LOW);
  Serial.begin(9600);
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveData);
  Wire.onRequest(sendData);
  Serial.println("Ready!");
}

void loop() {
  delay(100);
}

void receiveData(int byteCount) {
  while(Wire.available()) {
    number = Wire.read();
    Serial.print("Data received: ");
    Serial.println(number);
  }
  digitalWrite(8, HIGH);
  delay(5);
  digitalWrite(8, LOW);
}

void sendData() {
  Wire.write(number + 1);
}

