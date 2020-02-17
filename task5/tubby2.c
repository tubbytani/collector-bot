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
			lcd_string("N_");
			counter++;
			i++;
			forward();
			_delay_ms(200);
			stop();
			if (counter==3||9||11||17)  //if(final_array[i]>counter)//counter compare
			{   forward();
				_delay_ms(60);
				stop();
				right();
				_delay_ms(615);
				stop();
				left();
				_delay_ms(5);
				stop();
				//velocity(120,110);
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
			 if ( ((final_array[i]==5)&&((final_array[i+1])==15))||((final_array[i]==15)&&((final_array[i+1])==5))||((final_array[i]==13)&&((final_array[i+1])==7))||((final_array[i]==7)&&((final_array[i+1])==13))  )
			{   velocity(120,110);
				forward();
				_delay_ms(200);
				stop();
				zap();
			}
			return;
		}
	}
