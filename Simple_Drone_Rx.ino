#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(8, 9);
const byte address[6] = "00001";

struct JoystickData {
  int x;
  int y;
  int button;
};

const int X_CENTER = 508;
const int Y_CENTER = 520;

// ---- 4 PWM PINS ----
const int PWM1 = 3;
const int PWM2 = 5;
const int PWM3 = 6;
const int PWM4 = 10;

bool paused = false;
bool lastButton = 1;

int lockedPWM = 0;
int pwm = 0;

void setup() {
  Serial.begin(9600);

  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);
  pinMode(PWM3, OUTPUT);
  pinMode(PWM4, OUTPUT);

  radio.begin();
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(0, address);
  radio.startListening();

  Serial.println("Receiver Ready...");
}

void setAllPWM(int value) {
  analogWrite(PWM1, value);
  analogWrite(PWM2, value);
  analogWrite(PWM3, value);
  analogWrite(PWM4, value);
}

void loop() {

  if (radio.available()) {

    JoystickData joy;
    radio.read(&joy, sizeof(joy));

    // PACKET FILTER
    if (joy.x < 0 || joy.x > 1023 ||
        joy.y < 0 || joy.y > 1023 ||
        (joy.button != 0 && joy.button != 1)) 
    {
      Serial.println("BAD PACKET - ignored");
      return;
    }

    // BUTTON TOGGLE LOCK
    if (joy.button == 0 && lastButton == 1) {
      paused = !paused;

      if (paused) {
        lockedPWM = pwm;
        Serial.print("Locked PWM = ");
        Serial.println(lockedPWM);
      } else {
        Serial.println("Unlocked! PWM free");
      }
    }

    lastButton = joy.button;

    if (paused) {
      setAllPWM(lockedPWM);
      Serial.print("PWM = ");
      Serial.print(lockedPWM);
      Serial.println("  (Locked)");
      return;
    }

    // ----- JOYSTICK LOGIC -----
    pwm = 0;

    if (joy.x > X_CENTER && abs(joy.y - Y_CENTER) < 10) {
      pwm = map(joy.x, X_CENTER, 1023, 0, 255);
    }
    else if (joy.y > Y_CENTER && abs(joy.x - X_CENTER) < 10) {
      pwm = map(joy.y, Y_CENTER, 1023, 0, 255);
    }
    else if (joy.y < Y_CENTER && abs(joy.x - X_CENTER) < 10) {
      pwm = map(joy.y, Y_CENTER, 0, 0, 255);
    }
    else if (joy.x < X_CENTER && abs(joy.y - Y_CENTER) < 10) {
      pwm = 0;
    }

    setAllPWM(pwm);

    Serial.print("X=");
    Serial.print(joy.x);
    Serial.print("  Y=");
    Serial.print(joy.y);
    Serial.print("  PWM=");
    Serial.println(pwm);
  }
}
