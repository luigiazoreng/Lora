#ifndef MOTOR_H
#define MOTOR_H

class Motor
{
public:
    Motor();
    void set();
    void forward();
    void backward();
    void left();
    void right();
    void stop();

private:
    #define pinoMotor0 25
    #define pinoMotor1 26
    #define pinoMotor2 15
    #define pinoMotor3 14
};
#endif // MOTOR_H