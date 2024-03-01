#include <Wire.h>
#include "SparkFun_MMA8452Q.h"
#include <Servo.h>

MMA8452Q accel1;
MMA8452Q accel2;

Servo servo;

const int ServoPin = 9;
const int RelayPin = 3;
const float ACCEL_THRESH = 1.95;
const float X_THRESH = 1.95;
const float Y_THRESH = 1.8;
const float Z_THRESH = 1.8;

const float LOW_X_THRESH = 1.2;
const float LOW_Y_THRESH = 1.2;
const float LOW_Z_THRESH = 1.2;

// Bools to aid the accelerometer tai chi mode
bool taiChiActiveL = false;
bool taiChiActiveR = false;
unsigned long lastTaiChiActivationTimeL = 0;
unsigned long lastTaiChiActivationTimeR = 0;

void setup() {
  Serial.begin(9600);
  Serial.println("MMA8452Q Basic Reading Code!");

  Wire.begin();
  Serial.println("I2C Begun");

  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, LOW);
  Serial.println("Relay Set up");

  servo.attach(ServoPin);
    Serial.println("Servo Set up");

  Serial.println(accel1.begin());
  Serial.println(accel2.begin());

  while (!accel1.begin() || !accel2.begin(Wire, 0x1c)) {

    if (!accel1.begin()){
          Serial.println("Accelerometer 1 not detected.");
    }
     if (!accel2.begin()){
          Serial.println("Accelerometer 2 not detected.");
    }
  }
  Serial.println("Accelerometers detected!");

  accel1.setScale(SCALE_4G);
  accel2.setScale(SCALE_4G);



}

void loop() {

  if (accel1.available() && accel2.available()) {
    float x1 = accel1.getCalculatedX();
    float x2 = accel2.getCalculatedX();
    float y1 = accel1.getCalculatedY();
    float y2 = accel2.getCalculatedY();
    float z1 = accel1.getCalculatedZ();
    float z2 = accel2.getCalculatedZ();

    processAcceleration(x1, x2, y1, y2, z1, z2);
    printAccelerometerData(x1, x2, y1, y2, z1, z2);
  }
  else
  {
    Serial.print("Accel 1 availability : ");
    Serial.println(accel1.available());
    Serial.print("Accel 2 availability : ");
    Serial.println(accel2.available());
  }
}

void processAcceleration(float x1, float x2, float y1, float y2, float z1, float z2) {
  unsigned long currentTime = millis();

  if (x1 >= X_THRESH || y1 >= Y_THRESH || z1 >= Z_THRESH) {
    punchAir('L');
  }
  else if (x2 >= X_THRESH || y2 >= Y_THRESH || z2 >= Z_THRESH) {
    punchAir('R');
  }

  else if (x1 >= LOW_X_THRESH || y1 >= LOW_Y_THRESH || z1 >= LOW_Z_THRESH) {
    // Tai Chi Mode -  Uses low threshhold to hold on pump for a while until the threshold is lost

    if (!taiChiActiveL) {
      Serial.println("Achieved Tai Chi");
      lastTaiChiActivationTimeL = currentTime;
      taiChiActiveL = true;
    } else if (currentTime - lastTaiChiActivationTimeL >= 500) {
      Serial.println("100ms Tai chi");
      taiChi('L');
    } else {
      taiChiActiveL = false; // Reset Tai Chi status when below threshold
      digitalWrite(RelayPin, LOW);
      Serial.println("TAIL CHIO OFF");
    }
  }
  else if (x2 >= LOW_X_THRESH || y2 >= LOW_Y_THRESH || z2 >= LOW_Z_THRESH) {
    if (!taiChiActiveR) {
      Serial.println("Achieved Tai Chi");

      lastTaiChiActivationTimeR = currentTime;
      taiChiActiveR = true;
    } else if (currentTime - lastTaiChiActivationTimeR >= 500) {
      Serial.println("DONEEE Tai chi");
      taiChi('R');
    }
  }
  else {
    taiChiActiveR = false; // Reset Tai Chi status when below threshold
    digitalWrite(RelayPin, LOW);
    Serial.println("TAIL CHIO OFF");

  }
}


void printCalculatedAccels(bool number)
{
  if (number == 0) {
    Serial.print(accel1.cx, 3);
    Serial.print("\t");
    Serial.print(accel1.cy, 3);
    Serial.print("\t");
    Serial.print(accel1.cz, 3);
    Serial.print("\t");
  }
  else {
    Serial.print(accel2.cx, 3);
    Serial.print("\t");
    Serial.print(accel2.cy, 3);
    Serial.print("\t");
    Serial.print(accel2.cz, 3);
    Serial.print("\t");

  }
}
void punchAir(char direction) {
  servoMove(direction);
  pumpOn();
  Serial.println(direction);
}

void servoMove(char direction) {
  int angle = (direction == 'L') ? 40 : 100;
  Serial.print("Servo moving in ");
  Serial.println(angle);
  servo.write(angle);
}

void taiChi(char direction) {
  servoMove(direction);
  Serial.println(direction);
  digitalWrite(RelayPin, HIGH);
  Serial.println("Low Thresh");
  delay(500);


}
void pumpOn() {
  digitalWrite(RelayPin, HIGH);
  Serial.println("Pump On");
  delay(1000);
  digitalWrite(RelayPin, LOW);
  Serial.println("Pump Off");

}

void printAccelerometerData(float x1, float x2, float y1, float y2, float z1, float z2) {
  Serial.print(" x: ");
  Serial.print(x1, 3);
  Serial.print(" ");
  Serial.print(x2, 3);
  Serial.print("\t");

  Serial.print(" y: ");
  Serial.print(y1, 3);
  Serial.print(" ");
  Serial.print(y2, 3);
  Serial.print("\t");

  Serial.print(" z: ");
  Serial.print(z1, 3);
  Serial.print(" ");
  Serial.println(z2, 3);
}
//void loop() {
//}
