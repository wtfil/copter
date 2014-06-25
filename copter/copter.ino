#define CALIBRATION false


#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

// TODO move it to gyro.cpp file
MPU6050 acce;






class Angle {
  protected:
    float summ;
    int count;
    float angle;
  public:
  
    Angle() {
      summ = 0;
      count = 0;
      angle = 0;
    }
    
    void update (int gx, int dt) {
      float x = gx * 0.007629627;
      summ += x;
      count ++;
  
      if (count < 50) return;
      
      float realSpead = x - summ / count;
      
      if (abs(realSpead) <= 0.05) return;
  
      summ -= x;
      count --;
      realSpead = x - summ / count;
  
      angle += realSpead * dt / 1000;
  
    }
  
    float get () {
      return angle;
    }  
};


class Gyro {
  
  private:
    int startTime;
    int gx;
    int gy;
    int gz;
    Angle x;
    Angle y;
    
  public: 
    Gyro () {
      startTime = millis();
    }
    void init() {
      acce.initialize();
    }
  
    void test() {
      Serial.println(acce.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
    }
  
    void update() {
      acce.getRotation(&gx, &gy, &gz);
      int endTime = millis();
      int dt = endTime - startTime;
      startTime = endTime;
      
      x.update(gx, dt);
      y.update(gy, dt);
    }
  
    void get(float& ax, float& ay) {
      ax = x.get();
      ay = y.get();
    }
};


Gyro gyro;

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

  gyro.init();
} 
 
void loop() { 
  
  gyro.update();
  
  float x;
  float y;
  gyro.get(x, y);
  Serial.print(x); Serial.print("\t"); Serial.println(y);

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
  //delay(50);

}

#endif
