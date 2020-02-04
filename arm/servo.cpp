#include <Servo.h> 
int servoPin1 = 3;
int servoPin2 = 4; 
void setup() 
{
 Servo1.attach(servoPin1);
Servo2.attach(servoPin2);
 }
 void loop()
 { // Make servo go to 0 degrees 
 
for(int i = 0, j = 180 ; i<=180, j>=0; ++i, --j)
{

Servo1.write(i); delay(1000); 
 Servo2.write(j); delay (1000);
 Servo1.write(90); delay(1000); 
  }
