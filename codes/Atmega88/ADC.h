unsigned int ADC_DATA[5];	//Store the ADC input value
unsigned char SENSER_DATA[2];	//Value to change the Robot state through the ADC value, to send to the robot side STM32

unsigned char channel;		//Start the ADC convert and select the channel to judge the state
unsigned char ADC_flag;		//0:To start the ADC convert. 1: ADC convert done, can be stored.
volatile unsigned char ADC_TIMER;


/*void ADC_Init(void)
{
	ADMUX |= 0b01000000;
	ADCSRA |= 0b10000000;
}
*/

void ADC_Start(unsigned int ch)
{

	if(ADC_flag==0)
	{                                                                                                                                      
    
		ADMUX = 0b01100000;
		ADMUX |= ch; //Bit OR operator
	
		ADCSRA |= 0b01000000; // start conversion. prescaler 2

		while(!(ADCSRA & (1<<ADIF)));

		ADC_flag=1;
	}
	else
	{
		if(ADC_TIMER >=20)
		{
			ADC_DATA[ch] = ADCH;                                                                                        
			channel++;
			if(channel >=5) channel=0;
			ADC_TIMER = 0;
			ADC_flag=0;
		}
	}

}


/*
void ADC_Start(unsigned int ch)
{

	if(ADC_flag==0)
	{
		ADMUX |= 0b01100000;
		ADMUX |= ch; //Bit OR operator
	
		ADCSRA |= 0b01000000; // start conversion. prescaler 2

		while(!(ADCSRA & (1<<ADIF)));

		ADC_flag=1;
	}
	else
	{
		if(ADC_TIMER >=20)
		{
			ADC_DATA[ch] = ADCH;
			channel++;
			if(channel >=5) channel=0;
			ADC_TIMER = 0;
			ADC_flag=0;
		}
	}

}
*/

