
 * GccApplication13.c
 *
 * Created: 1/31/2020 11:12:30 PM
 * Author : new
 */ 

#define F_CPU 14745600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h> //included to support power function


#define RS 0
#define RW 1
#define EN 2
#define lcd_port PORTC

#define sbit(reg,bit)	reg |= (1<<bit)			// Macro defined for Setting a bit of any register.
#define cbit(reg,bit)	reg &= ~(1<<bit)		// Macro defined for Clearing a bit of any register.


void lcd_reset();
void lcd_init();
void lcd_wr_command(unsigned char);
void lcd_wr_char(char);
void lcd_line1();
void lcd_line2();
void lcd_string(char*);

unsigned int temp;
unsigned int unit;
unsigned int tens;
unsigned int hundred;
unsigned int thousand;
unsigned int million;


void port_init();
void timer5_init();
void velocity(unsigned char, unsigned char);
void motors_delay();
void white_line(void);
void fwd(void);
void bam(void);
void zoom(void);
void fwd1(void);

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned int value;
unsigned char sharp=0, distance=0, adc_reading=0;
unsigned char flag = 0;
int counter=2;
int final_array[]={1,2,3,4,0,7,8,13,7,6,0,15};
int new_array[]=  {1,2,3,4,6,15,0 ,0 ,6,5,0,0 };
int house_array[]={1,2,3,4,5,13,13,13,6,15,15,15};
int cm_array[]={1,2,3,4};
int i=3;
int j=0;
int l=0;
unsigned char left_sensor = 0;
unsigned char center_sensor = 0;
unsigned char right_sensor = 0;
unsigned char lvalue=0,rvalue=0;


//Function to configure LCD port
void lcd_port_config (void)
{
 DDRC = DDRC | 0xF7; //all the LCD pin's direction set as output
 PORTC = PORTC & 0x80; // all the LCD pins are set to logic 0 except PORTC 7
}

//ADC pin configuration
void lcd_set_4bit()
{
	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x30;				//Sending 3
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(5);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	_delay_ms(1);

	cbit(lcd_port,RS);				//RS=0 --- Command Input
	cbit(lcd_port,RW);				//RW=0 --- Writing to LCD
	lcd_port = 0x20;				//Sending 2 to initialise LCD 4-bit mode
	sbit(lcd_port,EN);				//Set Enable Pin
	_delay_ms(1);					//Delay
	cbit(lcd_port,EN);				//Clear Enable Pin

	
}

//Function to Initialize LCD
void lcd_init()
{
	_delay_ms(1);

	lcd_wr_command(0x28);			//LCD 4-bit mode and 2 lines.
	lcd_wr_command(0x01);
	lcd_wr_command(0x06);
	lcd_wr_command(0x0E);
	lcd_wr_command(0x80);
	
}

//Function to Write Command on LCD
void lcd_wr_command(unsigned char cmd)
{
	unsigned char temp;
	temp = cmd;
	temp = temp & 0xF0;
	lcd_port &= 0x0F;
	lcd_port |= temp;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
	
	cmd = cmd & 0x0F;
	cmd = cmd<<4;
	lcd_port &= 0x0F;
	lcd_port |= cmd;
	cbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}

//Function to Write Data on LCD
void lcd_wr_char(char letter)
{
	char temp;
	temp = letter;
	temp = (temp & 0xF0);
	lcd_port &= 0x0F;
	lcd_port |= temp;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);

	letter = letter & 0x0F;
	letter = letter<<4;
	lcd_port &= 0x0F;
	lcd_port |= letter;
	sbit(lcd_port,RS);
	cbit(lcd_port,RW);
	sbit(lcd_port,EN);
	_delay_ms(5);
	cbit(lcd_port,EN);
}
//Function to bring cursor at home position
void lcd_home()
{
	lcd_wr_command(0x80);
}

//Function to Print String on LCD
void lcd_string(char *str)
{
	while(*str != '\0')
	{
		lcd_wr_char(*str);
		str++;
	}
}

//Position the LCD cursor at "row", "column".
void lcd_cursor (char row, char column)
{
	switch (row) {
		case 1: lcd_wr_command (0x80 + column - 1); break;
		case 2: lcd_wr_command (0xc0 + column - 1); break;
		case 3: lcd_wr_command (0x94 + column - 1); break;
		case 4: lcd_wr_command (0xd4 + column - 1); break;
		default: break;
	}
}

//Function To Print Any input value up to the desired digit on LCD
void lcd_print (char row, char coloumn, unsigned int value, int digits)
{
	unsigned char flag=0;
	if(row==0||coloumn==0)
	{
		lcd_home();
	}
	else
	{
		lcd_cursor(row,coloumn);
	}
	if(digits==5 || flag==1)
	{
		million=value/10000+48;
		lcd_wr_char(million);
		flag=1;
	}
	if(digits==4 || flag==1)
	{
		temp = value/1000;
		thousand = temp%10 + 48;
		lcd_wr_char(thousand);
		flag=1;
	}
	if(digits==3 || flag==1)
	{
		temp = value/100;
		hundred = temp%10 + 48;
		lcd_wr_char(hundred);
		flag=1;
	}
	if(digits==2 || flag==1)
	{
		temp = value/10;
		tens = temp%10 + 48;
		lcd_wr_char(tens);
		flag=1;
	}
	if(digits==1 || flag==1)
	{
		unit = value%10 + 48;
		lcd_wr_char(unit);
	}
	if(digits>5)
	{
		lcd_wr_char('E');
	}
	
}
void adc_pin_config (void)
{
 DDRF = 0x00; 
 PORTF = 0x00;
 DDRK = 0x00;
 PORTK = 0x00;
}

//Function to configure ports to enable robot's motion
void motion_pin_config (void) 
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to Initialize PORTS
void port_init()
{
	lcd_port_config();
	adc_pin_config();
	motion_pin_config();	
}

void timer5_init()
{
	TCCR5B = 0x00;	//Stop
	TCNT5H = 0xFF;	//Counter higher 8-bit value to which OCR5xH value is compared with
	TCNT5L = 0x01;	//Counter lower 8-bit value to which OCR5xH value is compared with
	OCR5AH = 0x00;	//Output compare register high value for Left Motor
	OCR5AL = 0xFF;	//Output compare register low value for Left Motor
	OCR5BH = 0x00;	//Output compare register high value for Right Motor
	OCR5BL = 0xFF;	//Output compare register low value for Right Motor
	OCR5CH = 0x00;	//Output compare register high value for Motor C1
	OCR5CL = 0xFF;	//Output compare register low value for Motor C1
	TCCR5A = 0xA9;	/*{COM5A1=1, COM5A0=0; COM5B1=1, COM5B0=0; COM5C1=1 COM5C0=0}
 					  For Overriding normal port functionality to OCRnA outputs.
				  	  {WGM51=0, WGM50=1} Along With WGM52 in TCCR5B for Selecting FAST PWM 8-bit Mode*/
	
	TCCR5B = 0x0B;	//WGM12=1; CS12=0, CS11=1, CS10=1 (Prescaler=64)
}

void adc_init()
{
	ADCSRA = 0x00;
	ADCSRB = 0x00;		//MUX5 = 0
	ADMUX = 0x20;		//Vref=5V external --- ADLAR=1 --- MUX4:0 = 0000
	ACSR = 0x80;
	ADCSRA = 0x86;		//ADEN=1 --- ADIE=1 --- ADPS2:0 = 1 1 0
}

//Function For ADC Conversion
unsigned char ADC_Conversion(unsigned char Ch) 
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;  			
	ADMUX= 0x20| Ch;	   		
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for conversion to complete
	a=ADCH;
	ADCSRA = ADCSRA|0x10; //clear ADIF (ADC Interrupt Flag) by writing 1 to it
	ADCSRB = 0x00;
	return a;
}
unsigned int Sharp_conversion(unsigned char adc_reading)
{
	float distance;
	unsigned int distanceInt;
	distance = (int)(10.00*(2799.6*(1.00/(pow(adc_reading,1.1546)))));
	distanceInt = (int)distance;
	if(distanceInt>800)
	{
		distanceInt=800;
	}
	return distanceInt;
}

//Function To Print Sensor Values At Desired Row And Column Location on LCD
void print_sensor(char row, char coloumn,unsigned char channel)
{
	
	ADC_Value = ADC_Conversion(channel);
	lcd_print(row, coloumn, ADC_Value, 3);
}

//Function for velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;
 Direction &= 0x0F; 		// removing upper nibble for the protection
 PortARestore = PORTA; 		// reading the PORTA original status
 PortARestore &= 0xF0; 		// making lower direction nibble to 0
 PortARestore |= Direction; // adding lower nibble for forward command and restoring the PORTA status
 PORTA = PortARestore; 		// executing the command
}

void forward (void) 
{
  motion_set (0x05);
}

void stop (void)
{
  motion_set (0x00);
}
void back (void) //both wheels backward
{
	motion_set(0x0A);
}
void left (void) //Left wheel backward, Right wheel forward
{
	motion_set(0x09);
}
void right (void) //Left wheel forward, Right wheel backward
{
	motion_set(0x06);
}
void soft_left (void) //Left wheel stationary, Right wheel forward
{
	motion_set(0x04);
}
void soft_right (void) //Left wheel forward, Right wheel is stationary
{
	motion_set(0x02);
}

void soft_left_2 (void) //Left wheel backward, right wheel stationary
{
	motion_set(0x01);
}

void soft_right_2 (void) //Left wheel stationary, Right wheel backward
{
	motion_set(0x08);
}

void uturn(void)
{
	left();
	_delay_ms(600);
	stop();
}
void buzzer()
{
	DDRB=0x01;
	PORTB=0x01;
	_delay_ms(500);
	PORTB=0x00;
}
void pick(void)
{
	stop();
}
void place(void)
{
	stop();
}
/*void fwd_wls(void)
{
	unsigned char center_sensor;
	center_sensor = ADC_Conversion(2);

	//for l shape
	if (center_sensor>=5)
	{
	while (left_sensor<=5&& right_sensor<=5)
	{ 
		forward();
		if(left_sensor<=5&& right_sensor<=5 && center_sensor<=5)
		{
			stop();
		}
	}
	
	stop();
	}
}*/
void wall()
{   lcd_string("wall");
	/*velocity(110,100);
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while(center_sensor>=5||left_sensor>=5||right_sensor>=5)
	{
		forward();
		_delay_ms(100);
		stop();
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
	}
	stop();
	right();
	_delay_ms(40);
	stop();
	print_sensor(2,1,4);
	print_sensor(2,6,5);
	velocity(80,70);
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while(center_sensor>=5||left_sensor>=5||right_sensor>=5)
	{
		forward();
		_delay_ms(150);
		stop();
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
	}
	stop();
	_delay_ms(1000);
	forward();
	_delay_ms(2000);
	stop();*/
	/*center_sensor= ADC_Conversion(2);
	rvalue= ADC_Conversion(5);
	lvalue = ADC_Conversion(4);
	lcd_string("sharp");
	print_sensor(2,1,4);
	print_sensor(2,7,5);
	while ( (((lvalue<=50)&&(lvalue>=48)) || ((rvalue<=50)&&(rvalue>=48))) && (center_sensor<=4))
	{
		forward();
		_delay_ms(15);
		rvalue= ADC_Conversion(5);
		lvalue= ADC_Conversion(4);
	    center_sensor= ADC_Conversion(2);

		if((lvalue<=50)&&(lvalue>=48))
		{
			while ((lvalue<=50)&&(lvalue>=48))
			{
				right();
				rvalue= ADC_Conversion(5);
				lvalue = ADC_Conversion(4);
				center_sensor = ADC_Conversion(2);
			}
			stop();
			
		}
		if ((rvalue<=50)&&(rvalue>=48))
		{
			while ((rvalue<=50)&&(rvalue>=48))
			{
				left();
				rvalue= ADC_Conversion(5);
				lvalue = ADC_Conversion(4);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}
   stop();
	  }
	return;	
	velocity(110,100);
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while(center_sensor>=5||left_sensor>=5||right_sensor>=5)
	{
	forward();
	_delay_ms(150);
	stop();
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	}
	stop();
	_delay_ms(2000);
	right();
	_delay_ms(35);
	stop();
	lcd_string("kk");*/
	//buzzer();
	velocity(80,69);
	forward();
	_delay_ms(1000);
	stop();
	right();
	_delay_ms(20);
	stop();
  forward();
   _delay_ms(5000);
   stop();

	//right();
	//_delay_ms(40);
	//stop();
	//buzzer();
	
	return;
}
void dash()
{
	
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while ((left_sensor >=5) || (right_sensor>=5) || (center_sensor>=5))
	{
		forward();
		_delay_ms(30);
		stop();//---------idhar stop ka ana sure ni
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);

		if ((center_sensor <=4) && (left_sensor <=4))
		{
			while ((center_sensor <=4) && (left_sensor<=4))
			{
				soft_right();
				_delay_ms(100);
				stop();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}

		}

		if ((center_sensor <=4) && (right_sensor <=4))
		{
			while ((center_sensor <=4) && (right_sensor<=4))
			{
				soft_left();
				_delay_ms(100);
				stop();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
		}
		if ((center_sensor >=5) && (right_sensor>=5) && (left_sensor>=5))
		{
			while ((center_sensor>=5) && (right_sensor>=5) && (left_sensor>=5))
			{   
				i++;
				return;
			}
		}
	}
}
void boom(void)
{
	if (final_array[i]>house_array[i])
	{
		if (flag==0)
		{   forward();
			_delay_ms(50);
			stop();
			left();
			_delay_ms(600);
			stop();
			//counter--;
		}
		else
		{   forward();
			_delay_ms(50);
			stop();
			right();
			_delay_ms(600);
			stop();
			counter++;
		}
	}
	if(final_array[i]<house_array[i])
	{
		if (flag==0)
		{   forward();
			_delay_ms(50);
			stop();
			right();
			_delay_ms(600);
			stop();
			//counter--;
		}
		else
		{   forward();
			_delay_ms(50);
			stop();
			left();
			_delay_ms(600);
			stop();
			counter++;
		}
	}
	if((cm_array[j]==1)||(cm_array[j]==3))
	{
	zoom();	
	}
	if((cm_array[j]==2)||(cm_array[j]==4))
	{
		i++;
		bam();
	}
	
}
void zap()
{ if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5)))
	{
		if( (final_array[i]==5)&& (final_array[i+1]==15) )
		{
			counter=15;
		}
		else
		{
			counter=5;
		}
		lvalue=ADC_Conversion(5);
		rvalue=ADC_Conversion(4);
		print_sensor(1,1,4);
		print_sensor(1,1,5);
		//if(lvalue<50)
		{   lcd_string("zap");
			right();
			_delay_ms(455);
			stop();
			forward();
			_delay_ms(600);
			stop();
			right();
			_delay_ms(10);
			stop();
			forward();
			_delay_ms(2000);
			stop();
			
		}
		/*else
		{
			left();
			_delay_ms(300);
			stop();
		}*/
		wall();
		i++;
		fwd();
		buzzer();
		counter--;
		boom();
	}
	if ( ((final_array[i]==7)&&((final_array[i+1])==13))||((final_array[i]==13)&&((final_array[i+1])==7)))
	{  
		if( (final_array[i]==7)&& (final_array[i+1]==13) )
		{
			counter=13;
		}
		else
		{
			counter=7; flag=0;
		}
		lvalue=ADC_Conversion(5);
		rvalue=ADC_Conversion(4);
		if(lvalue<50)
		{
			right();
			_delay_ms(300);
			stop();
		}
		else
		{
			left();
			_delay_ms(300);
			stop();
		}
		forward();
		_delay_ms(40);
		stop();
		fwd();
		counter--;
		dash();
		boom();	
	}
	return;
}
void fwd(void)
{
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while ((left_sensor >=5) || (right_sensor >=5) || (center_sensor >=5))
	{
		forward();
		_delay_ms(15);//---------idhar ka sure ni ki delay ayega
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);

		if ((center_sensor <5) && (left_sensor <5))
		{
			while ((center_sensor <5) && (left_sensor <5))
			{
				right();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}
left_sensor = ADC_Conversion(1);
right_sensor = ADC_Conversion(3);
center_sensor = ADC_Conversion(2);
		if ((center_sensor <5) && (right_sensor <5))
		{
			while ((center_sensor <5) && (right_sensor<5))
			{
				left();
				
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		while(((center_sensor<5)&&(right_sensor<5)&&(left_sensor<5)))
		{
			/*if(l==0)
			{
			left();
			_delay_ms(5);
			stop();
			l=1;
			}
			else
			{
				right();
				_delay_ms(10);
				stop();
			}
	*/
			left();
			_delay_ms(5);
			stop();
	        
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
		}
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		while ((left_sensor >=5) && (right_sensor >=5) && (center_sensor >=5))
		{  //print_sensor(1,1,4);
			//print_sensor(2,1,5);
			counter++;
			forward();
			_delay_ms(5);
			stop();
			if (counter==3||9||11||17)  //if(final_array[i]>counter)//counter compare
			{   forward();
				_delay_ms(160);
				stop();
				right();
				_delay_ms(610);
				stop();
				left();
				_delay_ms(5);
				stop();
				velocity(120,110);
				if(right_sensor>=5||center_sensor>=5)
			{
				fwd();
			}
			else
			{
				left();
				_delay_ms(1200);
				stop();
				if(left_sensor>=5||center_sensor>=5)
				{
					fwd();
				}
			}
			}
			//return;
			/* if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5)))
			{   velocity(120,110);
				forward();
				_delay_ms(200);
				stop();
				zap();
			}*/
			return;
			/*white_line();*/
		}
	}
}
void fwd1(void)
{
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while ((left_sensor >=5) || (right_sensor >=5) || (center_sensor >=5))
	{
		forward();
		_delay_ms(15);//---------idhar ka sure ni ki delay ayega
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);

		if ((center_sensor <5) && (left_sensor <5))
		{
			while ((center_sensor <5) && (left_sensor <5))
			{
				right();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		if ((center_sensor <5) && (right_sensor <5))
		{
			while ((center_sensor <5) && (right_sensor<5))
			{
				left();
				
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		while((center_sensor<5)&&(right_sensor<5)&&(left_sensor<5))
		{
			stop();
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
		}
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		while ((left_sensor >=5) && (right_sensor >=5) && (center_sensor >=5))
		{
			counter--;
			forward();
			_delay_ms(5);
			stop();
			if (counter==3||9||11||17)  //if(final_array[i]>counter)//counter compare
			{   forward();
				_delay_ms(100);
				stop();
				right();
				_delay_ms(900);
				stop();
				if(right_sensor>=5||center_sensor>=5)
				{
					fwd1();
				}
				else
				{
					left();
					_delay_ms(1400);
					if(left_sensor>=5||center_sensor>=5)
					{
						fwd1();
					}
				}
				
				stop();
			}
			stop();
			soft_right();
			_delay_ms(5);
			stop();
			//return;
			if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5)))
			{   forward();
				_delay_ms(200);
				stop();
				zap();
			}
			return;//----------------------noooooooooooooooooooooooooooooooooooo bam m go
			/*white_line();*/
		}
	}
}
/*void white_line(void)
{
	if ((counter==9)||(counter==13)|| (counter==11)|| (counter==7))
	{
		left();
		_delay_ms(600);
		stop();
		
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		if((left_sensor>=5)||(right_sensor>=5)||(center_sensor>=5))
		{
			counter++;
			fwd_wls();
		}
		else
		{
			counter++;
			right();
			_delay_ms(1200);
			stop();
		}
		fwd_wls();
	}
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	if ((right_sensor>=5) && (left_sensor>=5) && (center_sensor>=5))
	{
		while((right_sensor<5)||(left_sensor<5))
		{
			fwd();
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
		}
		if((center_sensor<5) && (left_sensor<5) && (right_sensor<5))
		{
			counter++;
		}
		if(final_array[i]==10)
		{
			if(right_sensor<5)
			{
				right();
				_delay_ms(600);
				stop();
			}
			else
			{
				left();
				_delay_ms(600);
			}
			stop();
			place();
		}
	}	
}*/
void zoom(void)
{ 
	while (counter!=house_array[i])
	{   
		fwd1();
		i++;	
	}
		lvalue=ADC_Conversion(5);
		rvalue=ADC_Conversion(4);
		if(lvalue<50)
		{
			left();
			_delay_ms(600);
			stop();
			forward();
			_delay_ms(50);
			stop();
			j++;
			place();
			right();
			_delay_ms(1200);
			stop();
			
		}
		else
		if(rvalue<50)
		{
			right();
			_delay_ms(600);
			stop();
			forward();
			_delay_ms(50);
			stop();
			j++;
			place();
		}
		else
		{   
			forward();
			_delay_ms(100);
			stop();
			place();
			buzzer();
		}
		
		zap();
	/*if(final_array[i]-counter>0)
	{
		left();
		_delay_ms(600);
		stop();
		bam();
	}
	else
	if(counter-final_array[i]<0)
	{
	right();
	_delay_ms(600);
	stop();
	bam();
	}*/
}
void bam(void)
{
	while ( (counter!=final_array[i]) )
	{
		fwd();
	
	}

	/*while( (counter!=final_array[i])&&(j==2||j==3) )
	{
		fwd1();
	}*/
	
	if (counter==final_array[i])
	{  		 
		lvalue=ADC_Conversion(5);
		rvalue=ADC_Conversion(4);
		/*if (lvalue<=80)
		{
			left();
			_delay_ms(200);
			stop();
			flag=1;
			pick();
			uturn();
			_delay_ms(100);
			stop();
			fwd();
			counter--;
			boom();
			zoom();
		}*/
		
		//{if (rvalue<=200)
			lcd_string("node");
			//print_sensor(1,7,4);
			//print_sensor(2,7,5);
			forward();
			_delay_ms(25);
		    stop();
			right();
			_delay_ms(140);
			stop();
			back();
			_delay_ms(15);
			stop();
			_delay_ms(2000);
			stop();
			flag=0;
			//pick();
			//lcd_string("left");
			j++;
			left();
			_delay_ms(560);
			stop();
			_delay_ms(3000);
			//velocity(120,110);
			i++;
			final_array[i]=new_array[i];
			bam();
			return;
		//i++;
	}
	
}
void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	timer5_init();
	sei();   //Enables the global interrupts
}

//Main Function
int main()
{
	init_devices();
	lcd_set_4bit();
	lcd_init();
	velocity (150, 140);//right left
	
	while(1)
	{
        lcd_cursor(1,3);
        
		left_sensor = ADC_Conversion(3);	//Getting data of Left WL Sensor
		center_sensor = ADC_Conversion(2);	//Getting data of Center WL Sensor
		right_sensor = ADC_Conversion(1);	//Getting data of Right WL Sensor

		/*print_sensor(1,1,3);	//Prints value of White Line Sensor1
		print_sensor(1,5,2);	//Prints Value of White Line Sensor2
		print_sensor(1,9,1);	//Prints Value of White Line Sensor3
		*/
		while(1)
		{   _delay_ms(3000);
			velocity (150, 140);
			//fwd_wls();
				forward();
				_delay_ms(900);
				stop();
			right();
			_delay_ms(655);
			stop();
			_delay_ms(1000);
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
			//if( (left_sensor>=5)&&(right_sensor>=5)&&(center_sensor>=5) )
			//counter--;
			bam();
		}

	}
}
