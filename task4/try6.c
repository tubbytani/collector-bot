
#define F_CPU 14745600

#include<avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

unsigned char ADC_Conversion(unsigned char);
unsigned char ADC_Value;
unsigned char sharp, distance, adc_reading;
unsigned int value;
int flag=0;
int counter=0;
int final_array[]={1,2,3};
int cm_array[]={1,2,3};
int i;
unsigned char left_sensor, right_sensor, center_sensor;
unsigned char lsharp,lvalue,rvalue,rsharp;
void white_line(void);
int fwd(void);


void adc_pin_config (void)
{
	DDRF = 0x00; //set PORTF direction as input
	PORTF = 0x00; //set PORTF pins floating
	DDRK = 0x00; //set PORTK direction as input
	PORTK = 0x00; //set PORTK pins floating
}

void motion_pin_config (void) //Function to configure ports to enable robot's motion
{
 DDRA = DDRA | 0x0F;
 PORTA = PORTA & 0xF0;
 DDRL = DDRL | 0x18;   //Setting PL3 and PL4 pins as output for PWM generation
 PORTL = PORTL | 0x18; //PL3 and PL4 pins are for velocity control using PWM.
}

//Function to initialize ports
void init_ports()
{
 motion_pin_config();
 adc_pin_config();
}

// Timer 5 initialized in PWM mode for velocity control
// Prescaler:256
// PWM 8bit fast, TOP=0x00FF
// Timer Frequency:225.000Hz
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

unsigned char ADC_Conversion(unsigned char Ch)//This Function accepts the Channel Number and returns the corresponding Analog Value
{
	unsigned char a;
	if(Ch>7)
	{
		ADCSRB = 0x08;
	}
	Ch = Ch & 0x07;
	ADMUX= 0x20| Ch;
	ADCSRA = ADCSRA | 0x40;		//Set start conversion bit
	while((ADCSRA&0x10)==0);	//Wait for ADC conversion to complete
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

// Function for robot velocity control
void velocity (unsigned char left_motor, unsigned char right_motor)
{
	OCR5AL = (unsigned char)left_motor;
	OCR5BL = (unsigned char)right_motor;
}

//Function used for setting motor's direction
void motion_set (unsigned char Direction)
{
 unsigned char PortARestore = 0;

 Direction &= 0x0F; 			// removing upper nibble as it is not needed
 PortARestore = PORTA; 			// reading the PORTA's original status
 PortARestore &= 0xF0; 			// setting lower direction nibble to 0
 PortARestore |= Direction; 	// adding lower nibble for direction command and restoring the PORTA status
 PORTA = PortARestore; 			// setting the command to the port
}


void forward (void) //both wheels forward
{
  motion_set(0x05);
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

void stop (void)
{
  motion_set(0x00);
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
_delay_ms(5000);
PORTB=0x00;	
}
void pick(void)
{
	
}
void place(void)
{
	
}
void fwd_wls(void)
{
	unsigned char center_sensor;
	center_sensor = ADC_Conversion(2);

	//for l shape

	while (center_sensor != 0)
	{
		forward();
		center_sensor = ADC_Conversion(2);
	}
	stop();
}
void wall()
{
unsigned char c;
c = ADC_Conversion(2);
left_sensor = ADC_Conversion(5);
right_sensor = ADC_Conversion(6);
center_sensor = ADC_Conversion(4);
while (((left_sensor != 0) || (right_sensor != 0) || (center_sensor != 0)) && ((c != 255)))
{
	forward();
	left_sensor = ADC_Conversion(5);
	right_sensor = ADC_Conversion(6);
	center_sensor = ADC_Conversion(4);
	c = ADC_Conversion(2);

	if (left_sensor < 50)
	{
		while ((left_sensor < 50) || (center_sensor < 50))
		{
			soft_right();
			left_sensor = ADC_Conversion(5);
			right_sensor = ADC_Conversion(6);
			center_sensor = ADC_Conversion(4);
			c = ADC_Conversion(2);
		}
		stop();

	}

	if ((right_sensor < 50))
	{
		while ((right_sensor < 50) || (center_sensor < 50))
		{
			soft_left();
			left_sensor = ADC_Conversion(5);
			right_sensor = ADC_Conversion(6);
			center_sensor = ADC_Conversion(4);
			c = ADC_Conversion(2);
		}
		stop();

	}


}



	
}
void dash()
{
	
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while ((left_sensor != 0) || (right_sensor != 0) || (center_sensor != 0))
	{
		forward();
		_delay_ms(30);
		stop();//---------idhar stop ka ana sure ni
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);

		if ((center_sensor == 0) && (left_sensor == 0))
		{
			while ((center_sensor == 0) && (left_sensor == 0))
			{
				soft_right();
				_delay_ms(600);
				stop();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}

		}

		if ((center_sensor == 0) && (right_sensor == 0))
		{
			while ((center_sensor == 0) && (right_sensor == 0))
			{
				soft_left();
				_delay_ms(600);
				stop();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
		}
		if ((center_sensor == 255) && (right_sensor == 255) && (left_sensor == 255))
		{
			while ((center_sensor == 255) && (right_sensor == 255) && (left_sensor == 255))
			{
				return;
			}
		}
	}
	
}
void boom(void)
{
	if (final_array<cm_array)
	{
		if (flag==0)
		{
			left();
			_delay_ms(600);
			stop();
			counter--;
		}
		else
		{
			right();
			_delay_ms(600);
			stop();
			counter++;
		}
	}
	if(final_array>cm_array)
	{
		if (flag==0)
		{
			right();
			_delay_ms(600);
			stop();
			counter--;
		}
		else
		{
			left();
			_delay_ms(600);
			stop();
			counter++;
		}
	}
}
void zap()
{ if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5)))
	{
		
		 lvalue=ADC_Conversion(11);
		 rvalue=ADC_Conversion(12);
		 lsharp=Sharp_conversion(lvalue);
		 rsharp=Sharp_conversion(rvalue);
		 if(lsharp<50)
		 {
			 right();
			 _delay_ms(600);
			 stop();
		 }
		else
			 {
			 left();
			 _delay_ms(600);
			 stop();
			 }
		fwd();
		wall();
		boom(); 
	}
	if ( ((final_array[i]==7)&&((final_array[i+1])==13))||((final_array[i]==13)&&((final_array[i+1])==7)))
	{
		lvalue=ADC_Conversion(11);
		rvalue=ADC_Conversion(12);
		lsharp=Sharp_conversion(lvalue);
		rsharp=Sharp_conversion(rvalue);
		if(lsharp<50)
		{
			right();
			_delay_ms(600);
			stop();
		}
		else
		{
			left();
			_delay_ms(600);
			stop();
		}
		fwd();
		dash();
		boom();
		
	}
}

int fwd(void)
{
	unsigned char left_sensor, right_sensor, center_sensor;
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	while ((left_sensor != 0) || (right_sensor != 0) || (center_sensor != 0))
	{
		forward();
		//---------idhar ka sure ni ki delay ayega
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);

		if ((center_sensor == 0) && (left_sensor == 0))
		{
			while ((center_sensor == 0) && (left_sensor == 0))
			{
				soft_right();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}

		if ((center_sensor == 0) && (right_sensor == 0))
		{
			while ((center_sensor == 0) && (right_sensor == 0))
			{
				soft_left();
				left_sensor = ADC_Conversion(1);
				right_sensor = ADC_Conversion(3);
				center_sensor = ADC_Conversion(2);
			}
			stop();
		}
		while ((left_sensor != 0) && (right_sensor != 0) && (center_sensor != 0))
		{
			counter++;
			forward();
			_delay_ms(10);
			stop();
			zap();
			white_line();
			return ;
		}

	}

}
void white_line(void)
{
	if ((counter==9)||(counter==13)|| (counter==11)|| (counter==7))
	{
		left();
		_delay_ms(600);
		stop();
		
		left_sensor = ADC_Conversion(1);
		right_sensor = ADC_Conversion(3);
		center_sensor = ADC_Conversion(2);
		if((left_sensor!=0)||(right_sensor!=0)||(center_sensor!=0))
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
	if ((right_sensor!=0) && (left_sensor!=0) && (center_sensor==0))
	{
		while((right_sensor==0)||(left_sensor==0))
		{
			fwd();
			left_sensor = ADC_Conversion(1);
			right_sensor = ADC_Conversion(3);
			center_sensor = ADC_Conversion(2);
		}
		if((center_sensor==0) && (left_sensor==0) && (right_sensor==0))
		{
			counter++;
		}
		if(final_array[counter]==10)
		{
			if(right_sensor==0)
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
	
}
void zoom(void)
{  int lsharp,lvalue,rvalue,rsharp;
	while (counter!=0)//--------this is wrong
	{
     fwd();
	 lvalue=ADC_Conversion(11);
	 rvalue=ADC_Conversion(12);
	 lsharp=Sharp_conversion(lvalue);
	 rsharp=Sharp_conversion(rvalue);
	 if(lsharp<50)
	 {
		 left();
		 _delay_ms(600);
		 stop();
		 forward();
		 _delay_ms(100);
		 stop();
		 place();
	 }
	 if(rsharp<50)
	 {
		 right();
		 _delay_ms(600);
		 stop();
		 forward();
		 _delay_ms(100);
		 stop();
		 place();
	 }
	}
}
void bam(void)
{  
	while (counter!=final_array[i])
	{
		fwd();
	}
	if (counter==final_array[i])
	{   lvalue=ADC_Conversion(11);
		rvalue=ADC_Conversion(12);
		lsharp=Sharp_conversion(lvalue);
		rsharp=Sharp_conversion(rvalue);
		if (lsharp<50)
		{
			left();
			flag=1;
			pick();
			uturn();
			fwd();
			counter--;
			boom();
			zoom();
		}
		if (rsharp<50)
		{
		 right();	
		 _delay_ms(600);
		 stop();
		 flag=0;
		 pick();
		 uturn();
		 fwd();
		 counter--;
		 boom();
		 zoom();
		}
	i++;		
	}
	
}
void init_devices (void) //use this function to initialize all devices
{
	cli(); //disable all interrupts
	init_ports();
	timer5_init();
	adc_init();
	sei(); //re-enable interrupts
}

//Main Function
int main()
{
	init_devices();
	velocity (100, 117); //Set robot velocity here. Smaller the value lesser will be the velocity
						 //Try different values between 0 to 255
	while(1)
	{
	fwd_wls();
	left_sensor = ADC_Conversion(1);
	right_sensor = ADC_Conversion(3);
	center_sensor = ADC_Conversion(2);
	if( (left_sensor!=0)&&(right_sensor!=0)&&(center_sensor!=0) )
	counter--;
	bam();
	}

}

