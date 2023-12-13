#include "Arduino.h"
#include "Motor.h"

Motor::Motor()
{
    // Constructor implementation
}

void Motor::set()
{
    pinMode(pinoMotor0, OUTPUT);
    pinMode(pinoMotor1, OUTPUT);
    pinMode(pinoMotor2, OUTPUT);
    pinMode(pinoMotor3, OUTPUT);
    digitalWrite(pinoMotor1, LOW);
    digitalWrite(pinoMotor2, LOW);
    digitalWrite(pinoMotor3, LOW);
    digitalWrite(pinoMotor0, LOW);
}

void Motor::forward()
{
    // going foward
    digitalWrite(pinoMotor1, HIGH);
    digitalWrite(pinoMotor2, HIGH);
    digitalWrite(pinoMotor3, LOW);
    digitalWrite(pinoMotor0, LOW);
}
void Motor::backward()
{
    //going backwards
    digitalWrite(pinoMotor1, LOW);
    digitalWrite(pinoMotor2, LOW);
    digitalWrite(pinoMotor3, LOW);
    digitalWrite(pinoMotor0, LOW);
}

void Motor::left()
{
    // going left
    digitalWrite(pinoMotor1, LOW);
    digitalWrite(pinoMotor2, HIGH);
    digitalWrite(pinoMotor3, LOW);
    digitalWrite(pinoMotor0, HIGH);
}

void Motor::right()
{
    // going right
    digitalWrite(pinoMotor1, HIGH);
    digitalWrite(pinoMotor2, LOW);
    digitalWrite(pinoMotor3, HIGH);
    digitalWrite(pinoMotor0, LOW);
}
void Motor::stop()
{
    // stopped
    digitalWrite(pinoMotor1, LOW);
    digitalWrite(pinoMotor2, LOW);
    digitalWrite(pinoMotor3, HIGH);
    digitalWrite(pinoMotor0, HIGH);
}