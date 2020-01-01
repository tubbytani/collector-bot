/*
 * Eyantracb.c
 *
 * Created: 12/30/2019 3:50:18 PM
 * Author : new
 */ 
#define  F_CPU 14745600
#include <avr/io.h>
#include<util/delay.h>



void motor_init()
{
	
DDRA=0x0F;
PORTA=0x00;
	
}
void forward()
{   
	motor_init();
	PORTA=0x05;
}
void backward()
{
	
	motor_init();
	PORTA=0x0A;
}
void right_turn()
{
	motor_init();
	PORTA=0x06;
}
void left_turn()
{
motor_init();
PORTA=0x09;	
}
void buzzer()
{
	DDRB=0x01;
	PORTB=0x01;
	_delay_ms(2000);
	PORTB=0x00;
}
void stop()
{
	motor_init();
}
void velocity_init()
{
	DDRL=0x30;//4 and 5 pin
	PORTL=0x30;//pull down types
		
}
	
void velocity(const int speed1,const int speed2)
{
	
velocity_init();
OCR0A=speed1;//using 8bit timers
OCR0B=speed2;

}	
void soft_left()
{
	motor_init();
	PORTA=0x02;	
	
}
void soft_right()
{
	motor_init();
	PORTA=0x04;
}
int main(void)
{
	/* Replace with your application code */

	while (1)
	{
		forward();
		_delay_ms(1500);//as diameter is 5cm thus to cover 10cm it would require 1.5seconds
		left_turn();
		_delay_ms(500);
		forward();
		_delay_ms(1500);
		right_turn();
		_delay_ms(500);
		forward();
		_delay_ms(1500);
		backward();
		stop();
		buzzer();
		stop();
	}
}