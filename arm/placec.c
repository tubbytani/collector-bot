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
		_delay_ms(2000);
		servo_1_free();
		servo_2_free();
		servo_3_free();
		while(1); 
     
}
