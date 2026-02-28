#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 9);  // CE, CSN

const byte address[6] = "00001";

// Joystick pins
const int VRx = A0;
const int VRy = A1;
const int SW  = 7;

// Data structure to send
struct JoystickData {
  int x;
  int y;
  int button;
};

void setup() {
  Serial.begin(9600);

  pinMode(SW, INPUT_PULLUP);   // switch is active LOW
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();

  Serial.println("TX + Joystick Ready");
}

void loop() {
  JoystickData data;

  data.x = analogRead(VRx);
  data.y = analogRead(VRy);
  data.button = digitalRead(SW);  // 1 = not pressed, 0 = pressed

  bool ok = radio.write(&data, sizeof(data));

  Serial.print("Sending  X:");
  Serial.print(data.x);
  Serial.print("  Y:");
  Serial.print(data.y);
  Serial.print("  Button:");
  Serial.print(data.button);
  Serial.println(ok ? "  [OK]" : "  [FAILED]");

  delay(100);
}
