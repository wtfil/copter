#define CALIBRATION false

#include <Servo.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"

int MIN_SPEAD = 1000;
int MAX_SPEAD = 2000;
int M1_PIN = 6;
int M2_PIN = 7;
int M3_PIN = 8;
int M4_PIN = 9;

// TODO move it to gyro.cpp file

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
    MPU6050 acce;
    
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

class Motor {
  
  private:
    Servo m;
    int balanceSpead;
    
  public:
    Motor (int pin) {
      m.attach(pin);
      balanceSpead = 0;
    }
    
    void balance(int spead) {
      balanceSpead = spead;
    }
    
    void set(int spead) {
      m.writeMicroseconds(spead + balanceSpead);
    } 
};

Gyro gyro;
Motor m1(M1_PIN);
Motor m2(M2_PIN); 
Motor m3(M3_PIN);
Motor m4(M4_PIN);

String input;

#if CALIBRATION
void setup() {
  Serial.begin(9600);
  
  m1.set(MAX_SPEAD);
  m2.set(MAX_SPEAD);
  m3.set(MAX_SPEAD);
  m4.set(MAX_SPEAD);
  
  // Wait for input
  while (!Serial.available());
  Serial.read();
  Serial.println("Minimum 1");
  
  m1.set(MIN_SPEAD);
  m2.set(MIN_SPEAD);
  m3.set(MIN_SPEAD);
  m4.set(MIN_SPEAD);
  Serial.println("Minimum 2");
  
  */
}
void loop() {}

#else

void setup() { 
  Serial.begin(9600);
  while (!Serial.available());
  /*
  m1.attach(M1_PIN);
  m2.attach(M2_PIN);
  m3.attach(M3_PIN);
  m4.attach(M4_PIN);
  */
  gyro.init();
} 
 
int offsetX = 0; // m1 +, m3 -
int offsetY = 0; // m2 +, m4 -

void loop() { 
  
  gyro.update();
  
  float x;
  float y;
  gyro.get(x, y);
  
  if (x > 0) {
    offsetX -= 1;
  } else {
    offsetX += 1;
  }
  
  if (y > 0) {
    offsetY += 1;
  } else {
    offsetY -= 1;
  }
  
  if (offsetX > 0) {
    m1.balance(-offsetX);
    m3.balance(0);
  } else {
    m3.balance(offsetX);
    m1.balance(0);
  }
  
  if (offsetY > 0) {
    m2.balance(-offsetY);
    m4.balance(0);
  } else {
    m4.balance(offsetY);
    m2.balance(0);
  }
  
  //Serial.print(x); Serial.print("\t"); Serial.println(y);

  if (Serial.available() > 0) {
    Serial.println("available");
    char ch = Serial.read();
    
    if (ch != 13) {     
      input += ch;
    }  else {
      
      int val = input.toInt();
      Serial.println(val);
      input = "";
      
      m1.set(val);
      m2.set(val);
      m3.set(val);
      m4.set(val);
      
    }
    
  }


}

#endif
