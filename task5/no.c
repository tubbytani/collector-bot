

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
void servo_2(unsigned char);
void servo_3(unsigned char);
void servo_1(unsigned char );
void servo_1_free (void);void servo_2_free (void);void servo_3_free (void);
void init_devices (void);
void white_line(void);

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned int value;
unsigned char sharp=0, distance=0, adc_reading=0;
unsigned char flag = 0;
unsigned char flag3 = 0;
int counter=2;
int final_array[]={1,2,3,4,5,6,7,13,14,13,7,6,7,13,14,15,16,15,14,13,12,13,7};
int placearr[]={13,7,15,13,7};//,15,15,13,7,5};
int pickarr[]={4,14,6,16,12};
//int cm_array[]={1,2,3,4};
int i=1,k=0,v=0;//final array var,pick var,place var
int j=1;
int l=0;
int flag2=1;
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
//Configure PORTB 5 pin for servo motor 1 operation
void servo1_pin_config (void)
{
	DDRB  = 0xFF;  //making PORTB 5 pin output
	PORTB = PORTB | 0x20; //setting PORTB 5 pin to logic 1
}

//Configure PORTB 6 pin for servo motor 2 operation
void servo2_pin_config (void)
{
	DDRB  = DDRB | 0x40;  //making PORTB 6 pin output
	PORTB = PORTB | 0x40; //setting PORTB 6 pin to logic 1
}

//Configure PORTB 7 pin for servo motor 3 operation
void servo3_pin_config (void)
{
	DDRB  = DDRB | 0x80;  //making PORTB 7 pin output
	PORTB = PORTB | 0x80; //setting PORTB 7 pin to logic 1
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
	 servo1_pin_config(); //Configure PORTB 5 pin for servo motor 1 operation
	 servo2_pin_config(); //Configure PORTB 6 pin for servo motor 2 operation
	 servo3_pin_config(); //Configure PORTB 7 pin for servo motor 3 operation	
}
void timer1_init(void)
{
 TCCR1B = 0x00; //stop
 TCNT1H = 0xFC; //Counter high value to which OCR1xH value is to be compared with
 TCNT1L = 0x01;	//Counter low value to which OCR1xH value is to be compared with
 OCR1AH = 0x03;	//Output compare Register high value for servo 1
 OCR1AL = 0xFF;	//Output Compare Register low Value For servo 1
 OCR1BH = 0x03;	//Output compare Register high value for servo 2
 OCR1BL = 0xFF;	//Output Compare Register low Value For servo 2
 OCR1CH = 0x03;	//Output compare Register high value for servo 3
 OCR1CL = 0xFF;	//Output Compare Register low Value For servo 3
 ICR1H  = 0x03;	
 ICR1L  = 0xFF;
 TCCR1A = 0xAB; /*{COM1A1=1, COM1A0=0; COM1B1=1, COM1B0=0; COM1C1=1 COM1C0=0}
 					For Overriding normal port functionality to OCRnA outputs.
				  {WGM11=1, WGM10=1} Along With WGM12 in TCCR1B for Selecting FAST PWM Mode*/
 TCCR1C = 0x00;
 TCCR1B = 0x0C; //WGM12=1; CS12=1, CS11=0, CS10=0 (Prescaler=256)
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
void servo_1(unsigned char degrees)
{
	float PositionPanServo = 0;
	PositionPanServo = ((float)degrees / 1.86) + 35.0;
	OCR1AH = 0x00;
	OCR1AL = (unsigned char) PositionPanServo;
}
void servo_2(unsigned char degrees)
{
	float PositionTiltServo = 0;
	PositionTiltServo = ((float)degrees / 1.86) + 35.0;
	OCR1BH = 0x00;
	OCR1BL = (unsigned char) PositionTiltServo;
}

//Function to rotate Servo 3 by a specified angle in the multiples of 1.86 degrees
void servo_3(unsigned char degrees)
{
	float PositionServo = 0;
	PositionServo = ((float)degrees / 1.86) + 35.0;
	OCR1CH = 0x00;
	OCR1CL = (unsigned char) PositionServo;
}

//servo_free functions unlocks the servo motors from the any angle
//and make them free by giving 100% duty cycle at the PWM. This function can be used to
//reduce the power consumption of the motor if it is holding load against the gravity.

void servo_1_free (void) //makes servo 1 free rotating
{
	OCR1AH = 0x03;
	OCR1AL = 0xFF; //Servo 1 off
}

void servo_2_free (void) //makes servo 2 free rotating
{
	OCR1BH = 0x03;
	OCR1BL = 0xFF; //Servo 2 off
}

void servo_3_free (void) //makes servo 3 free rotating
{
	OCR1CH = 0x03;
	OCR1CL = 0xFF; //Servo 3 off
}

void pick(void)
{
	unsigned char v = 100,n=120,m=100;
	init_devices();
    forward();
    _delay_ms(235);
    stop();
	for (v = 100; v>15; v--)
	{
		servo_1(v);
		_delay_ms(50);
	}
	for(n=115; n>110;n--)
	{
		servo_2(n);
		_delay_ms(50);
	}
	for(m=70;m>0;m--)
	{
		servo_3(m);
		_delay_ms(50);
	}
	for(v=15; v<100;v++)
	{
		servo_1(v);
		_delay_ms(50);
	}
	
	_delay_ms(1000);
	servo_1_free();
	servo_2_free();
	servo_3_free();
}
void place(void)
	{
		unsigned char p= 100,q=110,r=0;
		
		init_devices();
		if((counter == 5) || (counter == 13) )
		{
			for(q=110;q<160;q++)
			{
				servo_2(q);
				_delay_ms(50);
			}
			for(p=100;p>50;p--)
			{
				servo_1(p);
				_delay_ms(50);
			}
			for(r=0;r<50;r++)
			{
				servo_3(r);
				_delay_ms(50);
			}
			for(p=50;p<100;p++)
			{
				servo_1(p);
				_delay_ms(50);
			}
		}
		else
		{
			for(p=100;p>40;p--)
			{
				servo_1(p);
				_delay_ms(50);
			}
			for(q=110;q<125;q++)
			{
				servo_2(q);
				_delay_ms(50);
			}
			for(r=0;r<50;r++)
			{
				servo_3(r);
				_delay_ms(50);
			}
			for(p=40;p<100;p++)
			{
				servo_1(p);
				_delay_ms(50);
			}
		}
		
		servo_1_free();
		servo_2_free();
		servo_3_free();
		
		
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
	stop();
	center_sensor= ADC_Conversion(2);
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
	lcd_string("kk");
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
*/
	
	return;
}
void dash(void)
{//no counter or variable change in this function
	
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while ((left_sensor >=5) || (right_sensor>=5) || (center_sensor>=5))
	{
		forward();
		_delay_ms(25);
		stop();
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);

		if ((center_sensor <=4) && (left_sensor <=4))
		{
			while ((center_sensor <=4) && (left_sensor<=4))
			{
				right();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}

		}stop();

		if ((center_sensor <=4) && (right_sensor <=4))
		{
			while ((center_sensor <=4) && (right_sensor<=4))
			{
				left();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
		}stop();
		if ((center_sensor >=5) && (right_sensor>=5) && (left_sensor>=5))
		{
			while ((center_sensor>=5) && (right_sensor>=5) && (left_sensor>=5))
			{   
				return;
			}
		}
	}
}
void boom(void)
{   
	if(((placearr[v]==13)||(placearr[v]==7)||(placearr[v]==5)||(placearr[v]==15))&&((counter ==13)||(counter == 7)))
	{
		return;
	}
	if ((final_array[i]>pickarr[k])&&(flag2==0))
	{
		if (flag==0)
		{   forward();
			_delay_ms(50);
			stop();
			right();
			_delay_ms(600);
			stop();
		}
		else
		{   forward();
			_delay_ms(50);
			stop();
			left();
			_delay_ms(600);
			stop();
		}
		j=0;
	}
	if((final_array[i]<pickarr[k])&&(flag2==0))
	{
		if (flag==0)
		{   forward();
			_delay_ms(50);
			stop();
			right();
			_delay_ms(600);
			stop();
		}
		else
		{   forward();
			_delay_ms(50);
			stop();
			left();
			_delay_ms(600);
			stop();
		}
		j=1;
	}
	
		if ((final_array[i]>placearr[v])&&(flag2==1))
	{
		 if(flag==0)
		 { forward();
			_delay_ms(30);
			stop();
			right();
			_delay_ms(600);
			stop();
	   }
	else
		{   forward();
			_delay_ms(50);
			stop();
			left();
			_delay_ms(600);
			stop();
		}
	j=1;
	}
  if((final_array[i]<placearr[v])&&(flag2==1))
	{
		if (flag==0)
		{   forward();
			_delay_ms(270);
			stop();
			//lcd_string("o");
			right();
			_delay_ms(550);
			stop();
			j=1;
		}
		else
		{  forward();
			_delay_ms(270);
			left();
			_delay_ms(600);
			stop();
			j=0;
		}
		
	}
	if(final_array[i]==10)
	{
		return;
	}
	if(flag2==0)
	{
		 bam();
	}
	else zoom();
	
}
void zap(void)
{
    
	/* if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5)))
	{
		if( (final_array[i]==5)&& (final_array[i+1]==15) )
		{
			counter=15;
		}
		else
		{
			counter=5;
		}
		print_sensor(1,1,4);
		print_sensor(1,1,5);
		if(lvalue<50)
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
			_delay_ms(1000);
			stop();
			
		}
		if(rvalue<=50)
		{ lcd_string("zap");
			left();
			_delay_ms(455);
			stop();
			forward();
			_delay_ms(600);
			stop();
			left();
			_delay_ms(10);
			stop();
			forward();
			_delay_ms(1000);
			stop();
			
		}
		
		wall();
		i++;
		fwd();
		counter--;
		boom();
	}*/
	if ( ((final_array[i]==7)&&((final_array[i+1])==13))||((final_array[i]==13)&&((final_array[i+1])==7)))
	{  
		if( (final_array[i]==7)&& (final_array[i+1]==13) )
		{
			counter=13;
		}
		else
		{
			counter=7;
		}
		if(j==1)
		{
            
			forward();
			_delay_ms(100);
			stop();
			right();
			_delay_ms(900);
			stop();
		}
		 if(j==0)
		{
			left();
			_delay_ms(450);
			stop();
		}
		
		forward();
		_delay_ms(50);
		stop();
		dash();
		lcd_string("d");
		//i++;
		left();
		_delay_ms(50);
		stop();
		counter--;
		fwd();
		forward();
		_delay_ms(150);
		stop();
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
		_delay_ms(25);
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
		{
			lcd_string("N");
			counter++;
			i++;
			forward();
			_delay_ms(200);
			stop();
			if ((counter==3)||(counter ==9)||(counter ==11)||(counter ==17))
			{   lcd_string("9");
                forward();
				_delay_ms(60);
				stop();
				right();
				_delay_ms(630);
				stop();
				/*left();
				_delay_ms(5);
				stop();*/
				if(((counter==3)||(counter==5)||(counter==11)||(counter==9))&&(final_array[i+1]!=10))
				{
					
					if(right_sensor>=5||center_sensor>=5)
					{fwd();}
					else
					{
						left();
						_delay_ms(1200);
						stop();
						if(left_sensor>=5||center_sensor>=5)
						{fwd();}
					}
					
				}
				if(((counter==11)||(counter==9))&&(final_array[i+1]==10))
				{ //lcd_string("l");
					stop();
					_delay_ms(1000);
					stop();
					right();
					_delay_ms(100);
					stop();
					forward();
					_delay_ms(170);
					stop();
					if(right_sensor>=5||center_sensor>=5||left_sensor>=5)
					{  // lcd_string("w");
						white_line();
					}
					else
					{
						left();
						_delay_ms(600);
						stop();
						lcd_string("2");
						if(left_sensor>=5||center_sensor>=5)
						{
							white_line();
						}
					}
				}
			}
			
			
			if ( ((final_array[i]==13)&&((final_array[i+1])==7))||((final_array[i]==7)&&((final_array[i+1])==13))  )
			{  
				
				zap();
			}
			/*if( ((final_array[i]==11)&&(final_array[i+1]=10))|| ((final_array[i]==9)&&(final_array[i+1]=10)))
			{  lcd_string("kkl");
				white_line();
			}*/
			return;
		    }
     return;
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
	_delay_ms(20);
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
	{  
		lcd_string("N");
		counter--;
		i++;
		forward();
		_delay_ms(200);
		stop();
		if ((counter==3)||(counter==9)||(counter==11)||(counter==17)) 
		{   forward();
			_delay_ms(60);
			stop();
			right();
			_delay_ms(615);
			stop();
			left();
			_delay_ms(5);
			stop();
	
			if((counter==3)||(counter==5))
			{
				
			     if(right_sensor>=5||center_sensor>=5)
			     {fwd();}
			    else
			    {
				left();
				_delay_ms(1200);
				stop();
				if(left_sensor>=5||center_sensor>=5)
				{fwd();}
			    }
		
			}
			if((counter==11)||(counter==9))
			{
				if(right_sensor>=5||center_sensor>=5)
				{white_line();}
				else
				{
					left();
					_delay_ms(1200);
					stop();
					if(left_sensor>=5||center_sensor>=5)
					{
						white_line();
					}
				}	
			}
		}
			
				
		if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5))||((final_array[i]==13)&&((final_array[i+1])==7))||((final_array[i]==7)&&((final_array[i+1])==13))  )
		{   velocity(120,110);
			forward();
			_delay_ms(200);
			stop();
			zap();
		}
		   // if( ((final_array[i]==11)&&(final_array[i+1]=10))|| ((final_array[i]==9)&&(final_array[i+1]=10)))
		   //{white_line();}
		return;
	  }



}	
}

void white_line(void)
{      
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		/*while (((left_sensor >=5) || (right_sensor >=5) )&&(center_sensor >=5))
		{
			forward();
			_delay_ms(20);
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
		}*/
		
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
			while(((center_sensor<5)||(right_sensor>=5)||(left_sensor>=5)))
			{
				forward();
				_delay_ms(30);
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
				
				if((center_sensor>=5) && (right_sensor>=5))
				{
					while((center_sensor>=5) && (right_sensor>=5))
					{
						left();
						left_sensor = ADC_Conversion(1);
						right_sensor = ADC_Conversion(3);
						center_sensor = ADC_Conversion(2);
					}
					stop();
				}
				if((center_sensor>=5) && (left_sensor>=5))
				{
					while((center_sensor>=5) && (left_sensor>=5))
					{
						right();
						left_sensor = ADC_Conversion(1);
						right_sensor = ADC_Conversion(3);
						center_sensor = ADC_Conversion(2);
					}
					stop();
				}
				
			if((center_sensor<5)&&(right_sensor<5)&&(left_sensor<5))
			{while((center_sensor<5)&&(right_sensor<5)&&(left_sensor<5))
			{
				if(j==1)
				{
					counter++;
				}
				else
				counter--;
				i++;
			
			if((counter==10)&&(flag3==0))
			{   forward();
				_delay_ms(30);
				stop();
				if(j==1)
				{  
					right();
					_delay_ms(500);
					stop();
				}
				else
				{
					left();
					_delay_ms(500);
					stop();
				}
					place();
					flag3=1;
					flag2=0;
					v++;
					right();
					_delay_ms(900);
					stop();
					boom();
					white_line();
					lcd_string("aa");
					forward();
					_delay_ms(50);
					stop();
					return;
				
			}
			}
			}
			}
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
			if((center_sensor>=5)&&(right_sensor>=5)&&(left_sensor>=5))
			{   if(j==1)
				{
					counter++;
				}
				else counter--;
				i++;
				if(flag2==0)
				{
					bam();
				}
				else
				{
					zoom();
				}
				return;
			}
			return;
}
void zoom(void)
{ 

while ( (counter!=placearr[v])&&(j==1))
{
velocity(140,130);
fwd();	
}

while( (counter!=placearr[v])&&(j==0))
{
	velocity(140,130);
	fwd1();
}
		lvalue=ADC_Conversion(5);
		rvalue=ADC_Conversion(4);
		if((placearr[v]==13)||(placearr[v]==7))
		{
			
			forward();
			_delay_ms(100);
			stop();
			place();
			flag2=0;
			v++;
			//lcd_string("p");
			right();
			_delay_ms(1000);
			stop();
			
		}
		else
		{
			
		if(j==1)
		{
			left();
			_delay_ms(600);
			stop();
			forward();
			_delay_ms(100);
			stop();
			place();
			flag2=0;
			v++;
			right();
			_delay_ms(1200);
			stop();
			
		}
		else
		if(j==0)
		{
			right();
			_delay_ms(600);
			stop();
			forward();
			_delay_ms(100);
			stop();
			
			place();
			flag2=0;
			v++;
			right();
			_delay_ms(1200);
			stop();
		}
		}
		boom();
		bam();
	
}
void bam(void)
{
	
	while ( (counter!=pickarr[k])&&(j==1)) 
	{
		fwd();
	
	}
	
	while( (counter!=pickarr[k])&&(j==0))
	{
		fwd1();
	}
	
	if (counter==pickarr[k])
	{  
		 
		   /*forward();
			_delay_ms(35);
			stop();
			left();
			_delay_ms(800);
			stop();
			forward();
			_delay_ms(50);
			stop();
			_delay_ms(2000);
			stop();
			flag=1;
			pick();
			flag2=1;
			k++;
			left();
			_delay_ms(1200);
			stop();
			boom();
			zoom();
			return;
			*/
			//if (rvalue<=20)
			//{
			forward();
			_delay_ms(30);
		    stop();
            right();
			_delay_ms(815);
			stop();
		    //forward();
		    //_delay_ms(50);
		    //stop();

			flag=0;
			pick();
			flag3=0;
			flag2=1;
			k++;
			left();
			_delay_ms(950);
			stop();
			//lcd_string("hi");
			boom();
      // velocity(120,110);
			zoom();
			return;
	     }
	}
	

void init_devices (void)
{
 	cli(); //Clears the global interrupts
	port_init();
	adc_init();
	timer1_init();
	timer5_init();
	sei();   //Enables the global interrupts
}

//Main Function
int main()
{
	init_devices();
	lcd_set_4bit();
	lcd_init();
	
	
		while(1)
		{  velocity (150, 140);//right left
			 _delay_ms(5000);
			forward();
			_delay_ms(900);
			stop();
			right();
			_delay_ms(645);
			stop();
			_delay_ms(1000);
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
			bam();
		}
}
