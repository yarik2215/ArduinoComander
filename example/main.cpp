#include <Arduino.h>
#include <Commander.h>

void led1();
void led2();
void led3();

Task tLed1(led1);
Task tLed2(led2);
Task tLed3(led3);

void setup() {
  commander.begin();
  pinMode(13, 1);
  pinMode(12, 1);
  pinMode(11, 1);
  Serial.begin(9600);
  Serial.println("start");

  commander.moveToQueue(&tLed1,2000);
  commander.moveToQueue(&tLed2,1000);
  commander.moveToQueue(&tLed3,100);
}

void loop() {
  // Serial.println("check");
  commander.check();

}

void led1()
{
  static bool state = false;
  state = !state;
  digitalWrite(13, state);
  commander.moveToQueue(&tLed1,500);
}

void led2()
{
  static bool state = false;
  state = !state;
  digitalWrite(12, state);
  commander.moveToQueue(&tLed2, 200);
}


void led3()
{
  static bool state = false;
  state = !state;
  digitalWrite(11, state);
  commander.moveToQueue(&tLed3, 1000);
}