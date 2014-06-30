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
    float error;

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
      float realSpead = x - summ / count;

      if (count < 100) {
        error += abs(realSpead); 
        return;
      }
      //Serial.print(gx); Serial.print("\t");
      if (abs(realSpead) <= error / count) return;

      summ -= x;
      count --;
      realSpead = x - summ / count;

      angle += realSpead * dt / 1000;

    }

    int get () {
      return angle;
    }
    
    void reset() {
      summ = error = count = angle = 0;
    }
};

// TODO move it to gyro.cpp file
class Gyro {
  
  private:
    int startTime;
    boolean enabled;
    int gx;
    int gy;
    int gz;
    Angle x;
    Angle y;
    MPU6050 acce;
    
  public: 
    Gyro () {
      startTime = millis();
      enabled = true;
    }
    void init() {
      acce.initialize();
    }
  
    void update() {
      acce.getRotation(&gx, &gy, &gz);
      //Serial.print("gx, gy:\t"); Serial.print(gx); Serial.println(gy);
      int endTime = millis();
      int dt = endTime - startTime;
      startTime = endTime;
      
      x.update(gx, dt);
      y.update(gy, dt);
    }
  
    void get(float& ax, float& ay) {
      if (!enabled) {
        ax = ay = 0;
        return;
      }
      ax = x.get();
      ay = y.get();
    }
    
    void reset() {
      x.reset();
      y.reset();
    }
    
    void off () {
      enabled = false;
    }
    
    void on () {
      enabled = true;
    }
};

class Motor {
  
  private:
    Servo m;
    String name;
    int spead;
    int pin;
    float balanceSpead;
    
  public:
    Motor (int p, String n) {
      balanceSpead = 0;
      spead = 0;
      pin = p;
      name = n;
    }
    
    void init() {
      m.attach(pin);
    }
    
    void balance(float angle) {
      balanceSpead = (1 - sin(angle * PI / 180) / 2);
      m.writeMicroseconds(get()); 
    }
    
    int get () {
      int diff = spead - MIN_SPEAD;
      return diff <= 0 ? spead : (MIN_SPEAD + diff * balanceSpead);
    }
    
    void set(int s) {
      spead = s;
      m.writeMicroseconds(get());
    }
};

Gyro gyro;
Motor m1(M1_PIN, "M1");
Motor m2(M2_PIN, "M2"); 
Motor m3(M3_PIN, "M3");
Motor m4(M4_PIN, "M4");

String input;

#if CALIBRATION
void setup() {
  Serial.begin(9600);
  
  m1.init();
  m2.init();
  m3.init();
  m4.init();
  
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
  
}
void loop() {}

#else

void setup() { 
  Serial.begin(9600);
  while (!Serial.available());

  m1.init();
  m2.init();
  m3.init();
  m4.init();
  gyro.init();
} 
 
void loop() { 
  
  gyro.update();
  
  float x;
  float y;
  gyro.get(x, y);
  
  if (x > 0) {
    m1.balance(x);
    m3.balance(0);
  } else {
    m3.balance(-x);
    m1.balance(0);
  }
  
  if (y > 0) {
    m2.balance(y);
    m4.balance(0);
  } else {
    m4.balance(-y);
    m2.balance(0);
  }
  
  Serial.print(x);Serial.print("\t");
  Serial.print(y);Serial.print("\t");
  Serial.print(m1.get());Serial.print("\t"); 
  Serial.print(m2.get());Serial.print("\t");
  Serial.print(m3.get());Serial.print("\t");
  Serial.print(m4.get());Serial.print("\t");
  Serial.println();

  if (Serial.available() > 0) {    
    char ch = Serial.read();
    
    if (ch != 13) {     
      input += ch;
    }  else {
      int val = input.toInt();
      if (String("reset") == input) {
        Serial.println("reseting..");
        gyro.reset();
      } else if (String("off") == input) {
        Serial.println("disable..");
        gyro.off();
      } else if (String("on") == input) {
        Serial.println("enable..");
        gyro.on();
      } else if (val >= MIN_SPEAD && val <= MAX_SPEAD) {
        Serial.print("writing:\t"); Serial.println(val);
        m1.set(val);
        m2.set(val);
        m3.set(val);
        m4.set(val);
      } else {
        Serial.println("Wrong command");
      }
      input = "";
    }
    
  }
  //delay(50);

}

#endif
