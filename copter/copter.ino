#define CALIBRATION false

#include <Servo.h> 


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

  //start();
} 
 
void loop() { 

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
