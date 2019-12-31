/*
 * Eyantracb.c
 *
 * Created: 12/30/2019 3:50:18 PM
 * Author : new
 */ 

#include <avr/io.h>
#include<util/delay.h>

int main(void)
{
    /* Replace with your application code */
	
    while (1) 
    {
		forward();
		left_turn();
		forward();
		right_turn();
		forward();
		backward();
    }
}

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
