#define CALIBRATION false

#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// TODO move it to gyro.cpp file
MPU6050 acce;

int startTime = millis();
float summ = 0;
int count = 0;
float angle = 0;

float realGx (int gx, int dt) {
  float x = gx * 0.007629627;
  summ += x;
  count ++;
  
  if (count < 50) return angle;
  
  float realSpead = x - summ / count;
  //Serial.print(dt); Serial.print("\t");
  //Serial.println(realSpead);
  
  if (realSpead >= -0.03 && realSpead <= 0.03) return angle;
  
  summ -= x;
  count --;
  realSpead = x - summ / count;
  
  angle += realSpead * dt / 1000;
  
  return angle;
}

class Gyro {
  public: 
    static void init() {
      acce.initialize();
    }
  
    static void test() {
      Serial.println(acce.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    }
  
    static void update() {
      int gx; int gy; int gz;
      acce.getRotation(&gx, &gy, &gz);
      int endTime = millis();
      int dt = endTime - startTime;
      startTime = endTime;
      
      realGx(gx, dt);
    }
  
    static float get() {
      return angle;
    }
};


int MIN_SPEAD = 1000;
int MAX_SPEAD = 2000;
int M1_PIN = 6;
int M2_PIN = 7;
int M3_PIN = 8;
int M4_PIN = 9;
 
Servo m1; 
Servo m2; 
Servo m3;
Servo m4; 

String input;

#if CALIBRATION
void setup() {
  Serial.begin(9600);
  
  m1.attach(M1_PIN);
  m2.attach(M2_PIN);
  m3.attach(M3_PIN);
  m4.attach(M4_PIN);
  m1.writeMicroseconds(MAX_SPEAD);
  m2.writeMicroseconds(MAX_SPEAD);
  m3.writeMicroseconds(MAX_SPEAD);
  m4.writeMicroseconds(MAX_SPEAD);
  
  // Wait for input
  while (!Serial.available());
  Serial.read();
  Serial.println("Minimum 1");
  
  m1.writeMicroseconds(MIN_SPEAD);
  m2.writeMicroseconds(MIN_SPEAD);
  m3.writeMicroseconds(MIN_SPEAD);
  m4.writeMicroseconds(MIN_SPEAD);
  Serial.println("Minimum 2");
}
void loop() {}

#else

void setup() { 
  Serial.begin(9600);
  m1.attach(M1_PIN);
  m2.attach(M2_PIN);
  m3.attach(M3_PIN);
  m4.attach(M4_PIN);

  Gyro::init();
} 
 
void loop() { 
  
  Gyro::update();
  
  float x = Gyro::get();
  Serial.println(x);

  if (Serial.available() > 0) {
    char ch = Serial.read();

    if (ch != 10) {
      input += ch;
    }  else {
      int val = input.toInt();
      Serial.println(val);
      input = "";
      
      m1.writeMicroseconds(val);
      m2.writeMicroseconds(val);
      m3.writeMicroseconds(val);
      m4.writeMicroseconds(val);
    }
    
  }
  delay(50);
  //test();

}

#endif
