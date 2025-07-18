/*

void DetermineArm(unsigned char n)
{
	switch(n)
	{
		case 5 :{ //손목
					if( DataArm[n] >= 2 )
					{
						TIM2->CCR1=TIM2_DUTY1;
					}
					else if( DataArm[n] >= 1)
					{
						TIM2->CCR1=TIM2_DUTY2;
					}
					else
					{
						TIM2->CCR1=TIM2_DUTY3;
					}
				}
		case 6 :{ //팔꿈치
					if( DataArm[n] >= 2 )
					{
						TIM2->CCR2=TIM2_DUTY1;
					}
					else if( DataArm[n] >= 1)
					{
						TIM2->CCR2=TIM2_DUTY2;
					}
					else
					{
						TIM2->CCR2=TIM2_DUTY3;
					}					
				}
		case 7 :{ //어깨1
					if( DataArm[n] >= 2 )
					{
						TIM2->CCR3=TIM2_DUTY1;
					}
					else if( DataArm[n] >= 1)
					{
						TIM2->CCR3=TIM2_DUTY2;
					}
					else
					{
						TIM2->CCR3=TIM2_DUTY3;
					}
				}
		case 8 :{ //어깨2
					if( DataArm[n] >= 2 )
					{
						TIM2->CCR4=TIM2_DUTY1;
					}
					else if( DataArm[n] >= 1)
					{
						TIM2->CCR4=TIM2_DUTY2;
					}
					else
					{
						TIM2->CCR4=TIM2_DUTY3;
					}
				}	
	}	
}

*/
    /*
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
	*/




#include "stm32f10x.h"
#include <stdio.h>

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure; 
																			  
volatile unsigned int Timer3_Counter = 0;	

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f) 
#endif /* __GNUC__ */

#include "stm32f10x.h"
 
#define SET 1
#define CLEAR 0
 
//survo motor => 20000 - expected PWM value (pass the NOT gate after through the photocoupler)
#define TIM2_DUTY1 20000-1000 //	+90
#define TIM2_DUTY2 20000-1500 //	  0
#define TIM2_DUTY3 20000-2000 //	-90
// #define TIM2_DUTY4 20000-2500 // not used
 
//preprocessing & UART data processing variable
#define DATA_IS_FINGER 0x33;
#define DATA_IS_ARM 0x55;
#define DATA_FINGER 1;
#define DATA_ARM 2;

 
unsigned int WhosData=0; //Check which Atmega88 sent the value through RX pin (1 or 2)
unsigned int DataFinger[6]={0,}; // recieved finger data array
unsigned int DataArm[4]={0,}; // recieved arm data array
unsigned int DataCount=0;// variable to manage the array input
unsigned int DetermineValue[9]={255,};//judge the data array of finger & arm, and save here (0,1,2,3,4 for finger + 5,6,7,8 for arm)
 											 
//variable for timer
int FingerDC_Now[6]={0,};

/*
set the DC motor activation time through setting the data.
ex: if the data SET is 100, increase the FingerDC_NOW till it matches FingerDC_SET(=100) and stop.
Decrease the value to 0 for the opposite case.
*/

volatile unsigned int TIME_TEST = 0;




//-----------------------------------------------------------
//		GPIO(PIN&PORT) setting
//-----------------------------------------------------------
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/// TIMER4 (TEST) CH 1,2,3,4
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	/// TIMER5 CH 1,2,3,4
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
 
	/// PC13 TEST LED
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	
	/// FINGER (PC0,1,2,3 + PE0,1,2,3,4,5)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
 
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
 
	/// PC12 TEST SWITCH_INPUT
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	//GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
 
	/// USART3_TX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);   
	 
	/// USART3_RX
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
}







//-----------------------------------------------------------
//		NVIC setting (set the Vector Table location, Interrupt Handler setting)
//-----------------------------------------------------------
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
  	/* Configure the NVIC Preemption Priority Bits */  
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	#ifdef  VECT_TAB_RAM  
	  /* Set the Vector Table base location at 0x20000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	  /* Set the Vector Table base location at 0x08000000 */ 
	  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	/* NVIC_PriorityGroup_1: 1 bits for pre-emption priority
                                 3 bits for subpriority	*/ 
	

	//USART3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel= USART3_IRQn;			// USART3 Global Interrupt
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// interrupt priority (p1)
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			// interrupt priority sub (p2)
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				// interrupt enable
	NVIC_Init(&NVIC_InitStructure);
	
	//TIM2(PWM)
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;                   
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelSubPriority	= 2;        
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//TIM3
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);   
}



//-----------------------------------------------------------
// 		CLOCK source
//-----------------------------------------------------------
void RCC_Configuration(void)
{
	SystemInit();	
	RCC_APB2PeriphClockCmd(	
						//	RCC_APB2Periph_USART1 |
							RCC_APB2Periph_GPIOA | 
							RCC_APB2Periph_GPIOB | 
							RCC_APB2Periph_GPIOC | 
							RCC_APB2Periph_GPIOD | 
							RCC_APB2Periph_GPIOE | 
							RCC_APB2Periph_AFIO
							, ENABLE );
	
	RCC_APB1PeriphClockCmd( 
							RCC_APB1Periph_TIM2 |
							RCC_APB1Periph_TIM3 |
							RCC_APB1Periph_USART3 
							, ENABLE );

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
}




//------------------------------------------------------------
//		USART3 setting
//-----------------------------------------------------------
void USART3_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate            = 9600 ;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_Odd;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);      
/*	DIR485_L;  */
	USART_Cmd(USART3, ENABLE);
}



//-----------------------------------------------------------
//		TIM2, TIM3 setting
//-----------------------------------------------------------
void TIMER_Configuration(void)
{
	// 1ms
	TIM_TimeBaseStructure.TIM_Period=1000-1; // period 
	TIM_TimeBaseStructure.TIM_Prescaler=72-1; // prescaler
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_ARRPreloadConfig(TIM3, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);






	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//TIMx->CR1 (Overflow event)
	TIM_TimeBaseStructure.TIM_Prescaler =72-1;					//72Mhz / 72 => 1MHz
	TIM_TimeBaseStructure.TIM_Period =20000-1;					//1MHz / 20000 => 50Hz (=20ms)
	TIM_TimeBaseStructure.
	TIM_ClockDivision =0;					//divide clock? NO (TIM_CKD_DIV1;//TIMx->CR1=0;)
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	//OC2CE OC2M[2:0] OC992PE OC2FE CC2S[1:0]OC1CE OC1M[2:0] OC1PE OC1FE CC1S[1:0]
	TIM2->CCMR1=0X0068|0X6800;				// TIM_OCMode_PWM1    CH1,CH2 //TIM_OC4PreloadConfig
	TIM2->CCMR2=0X0068|0X6800;				// TIM_OCMode_PWM1    CH3,CH4 //TIM_OC4PreloadConfig
	
	//Reserved CC4P CC4E CC3NP CC3NE CC3P CC3E CC2NP CC2NE CC2P CC2E CC1NP CC1NE CC1P CC1E  
	TIM2->CCER=0X0001|0X0010|0X0100|0X1000; 	//CC1P(1)=TIM_OCPolarity_High : CC1E(0)=TIM_OutputState_Enable

	/*
	TIM2->CCR1=TIM2_DUTY1;
	TIM2->CCR2=TIM2_DUTY2;  		
	TIM2->CCR3=TIM2_DUTY3;  
	TIM2->CCR4=TIM2_DUTY4; 
	*/	

	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);//TIM4 counter enable   

	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); 
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	TIM_Cmd(TIM2, ENABLE);	
}



//-----------------------------------------------------------
//		User_Fx								 
//-----------------------------------------------------------
void DetermineFinger(unsigned char n)
{
		 if( 50 < DataFinger[n] && DataFinger[n] <= 111) // if the finger is bent
	{
		DetermineValue[n] = 1; // start motor activation.
	}
	else if( 111 < DataFinger[n] && DataFinger[n] < 180) // if the finger is unfurl
	{
		DetermineValue[n] = 0; // active the motor to the otherwise.
	}
	else
	{
		DetermineValue[n] = 2;
		//exception : not defined value for action. value = 2 is the exception.
	}
}	


//1
void MoveFinger1(void)
{
	
	if(DetermineValue[1]==1)					  //the sensor is bent
	{
		if(FingerDC_Now[1] < 1500)				  //the hand(last status) is unfurl
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_0);		   // motor active forward to bend the finger
  			GPIO_ResetBits(GPIOD,GPIO_Pin_1);
		}
	}
	else if(DetermineValue[1]==0)				   // the sensor is unfurl
	{
		if(FingerDC_Now[1] > 1000)				   // the hand(last status) is bent
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_1);		   // motor active backward to unfold the finger
  			GPIO_ResetBits(GPIOD,GPIO_Pin_0);
		}
	}
	else											// idle state
	{
	}
}


//2
void MoveFinger2(void)							  // same with Movefinger1, till 5
{
	
	if(DetermineValue[2]==1)
	{
		if(FingerDC_Now[2] < 1500)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_2);
  			GPIO_ResetBits(GPIOD,GPIO_Pin_3);
		}
	}
	else if(DetermineValue[2]==0)
	{
		if(FingerDC_Now[2] > 800)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_3);
  			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
		}
	}
	else
	{
	}
}


//3
void MoveFinger3(void)
{
	
	if(DetermineValue[3]==1)
	{
		if(FingerDC_Now[3] < 1500)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_4);
  			GPIO_ResetBits(GPIOD,GPIO_Pin_5);
		}
	}
	else if(DetermineValue[3]==0)
	{
		if(FingerDC_Now[3] > 1000)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_5);
  			GPIO_ResetBits(GPIOD,GPIO_Pin_4);
		}
	}
	else
	{
	}
}


//4
void MoveFinger4(void)
{
	
	if(DetermineValue[4]==1)
	{
		if(FingerDC_Now[4] < 1500)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_6);
  			GPIO_ResetBits(GPIOD,GPIO_Pin_7);
		}
	}
	else if(DetermineValue[4]==0)
	{
		if(FingerDC_Now[4] > 500)
		{
			GPIO_SetBits(GPIOD,GPIO_Pin_7);
  			GPIO_ResetBits(GPIOD,GPIO_Pin_6);
		}
	}
	else
	{
	}
}


//5
void MoveFinger5(void)
{
	
	if(DetermineValue[5]==1)
	{
		if(FingerDC_Now[5] < 1500)
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_0);
  			GPIO_ResetBits(GPIOE,GPIO_Pin_1);
		}
	}
	else if(DetermineValue[5]==0)
	{
		if(FingerDC_Now[5] > 1000)
		{
			GPIO_SetBits(GPIOE,GPIO_Pin_1);
  			GPIO_ResetBits(GPIOE,GPIO_Pin_0);
		}
	}
	else
	{
	}
}






//-----------------------------------------------------------
//		M A I N
//-----------------------------------------------------------
void delay_ms(unsigned int del) 
{
	Timer3_Counter = 0; 
	while(Timer3_Counter < del); 
}


void Init_All_Periph(void)
{
	RCC_Configuration();	
	GPIO_Configuration();
	NVIC_Configuration();
	USART3_Configuration();
	TIMER_Configuration();
}

//////////////////////////////////////////////////////////////

int main(void)
{
	Init_All_Periph();
	GPIO_SetBits(GPIOC,GPIO_Pin_13); // if the signal from bluetooth is 0x33, PC13 LED turns ON (currently OFF)

 	while(1)
	{
		DetermineFinger(1);
		MoveFinger1();

		DetermineFinger(2);
		MoveFinger2();
				
		DetermineFinger(3);
		MoveFinger3();
		
		DetermineFinger(4);
		MoveFinger4();

		DetermineFinger(5);
		MoveFinger5();
  	}
}






//-----------------------------------------------------------
//		interrupt handler (TIM3, USART3)
//-----------------------------------------------------------					 
void TIM3_IRQHandler(unsigned int n) 
{ 
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) 
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		Timer3_Counter++;
		
		//finger1
		if(DetermineValue[1]==1) //	finger is unfurl
		{							 
   			if( FingerDC_Now[1] < 1500)	// bend the finger
   			{
      			FingerDC_Now[1]++;
   			}
  			else						// stop if its bended all
   			{							
      			GPIO_ResetBits(GPIOD,GPIO_Pin_0);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_1);
   			}
		}
		else if(DetermineValue[1]==0)	// finger is bended
		{
   			if( FingerDC_Now[1] > 0)	// unfold the finger
   			{
      			FingerDC_Now[1]--;
   			}
   			if( FingerDC_Now[1] <= 1000) //	untill it's done
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_0);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_1);
   			}
		}

		if( FingerDC_Now[1] > 1500 || FingerDC_Now[1] <= 1000) // to make no movement in the initial stage.
		{      			
			GPIO_ResetBits(GPIOD,GPIO_Pin_0);
			GPIO_ResetBits(GPIOD,GPIO_Pin_1);
		}


		//finger2								   same as finger1 till 5
		if(DetermineValue[2]==1)
		{
   			if( FingerDC_Now[2] < 1500)
   			{
      			FingerDC_Now[2]++;
   			}
   			else
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_3);
   			}
		}
		else if(DetermineValue[2]==0)
		{
   			if( FingerDC_Now[2] > 0)
   			{
      			FingerDC_Now[2]--;
   			}
   			if( FingerDC_Now[2] <= 800)
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_3);
   			}
		}	   

		if( FingerDC_Now[2] > 1500 || FingerDC_Now[2] <= 800)
		{      			
			GPIO_ResetBits(GPIOD,GPIO_Pin_2);
			GPIO_ResetBits(GPIOD,GPIO_Pin_3);
		}


		//finger3
		if(DetermineValue[3]==1)
		{
   			if( FingerDC_Now[3] < 1500)
   			{
      			FingerDC_Now[3]++;
   			}
   			else
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_4);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_5);
   			}
		}
		else if(DetermineValue[3]==0)
		{
   			if( FingerDC_Now[3] > 0)
   			{
      			FingerDC_Now[3]--;
   			}
   			if( FingerDC_Now[3] <= 1000)
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_4);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_5);
   			}
		}

		if( FingerDC_Now[3] > 1500 || FingerDC_Now[3] <= 1000)
		{      			
			GPIO_ResetBits(GPIOD,GPIO_Pin_4);
			GPIO_ResetBits(GPIOD,GPIO_Pin_5);
		}


		//finger4 
		if(DetermineValue[4]==1)
		{
   			if( FingerDC_Now[4] < 1500)
   			{
      			FingerDC_Now[4]++;
   			}
   			else if(DetermineValue[4]==0)
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_6);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_7);
   			}
		}
		else if(DetermineValue[4]==0)
		{
   			if( FingerDC_Now[4] > 0)
   			{
      			FingerDC_Now[4]--;
   			}
   			if( FingerDC_Now[4] <= 500)
   			{
      			GPIO_ResetBits(GPIOD,GPIO_Pin_6);
      			GPIO_ResetBits(GPIOD,GPIO_Pin_7);
   			}
		}
		
		if( FingerDC_Now[4] > 1500 || FingerDC_Now[4] <= 500)
		{      			
			GPIO_ResetBits(GPIOD,GPIO_Pin_6);
			GPIO_ResetBits(GPIOD,GPIO_Pin_7);
		}


		//finger5
		if(DetermineValue[5]==1)
		{
   			if( FingerDC_Now[5] < 1500)
   			{
      			FingerDC_Now[5]++;
   			}
   			else
   			{
      			GPIO_ResetBits(GPIOE,GPIO_Pin_0);
      			GPIO_ResetBits(GPIOE,GPIO_Pin_1);
   			}
		}
		else if(DetermineValue[5]==0)
		{
   			if( FingerDC_Now[5] > 0)
   			{
      			FingerDC_Now[5]--;
   			}
   			if( FingerDC_Now[5] <= 1000)
   			{
      			GPIO_ResetBits(GPIOE,GPIO_Pin_0);
      			GPIO_ResetBits(GPIOE,GPIO_Pin_1);
   			}
		}
	
		if( FingerDC_Now[5] > 1500 || FingerDC_Now[5] <= 1000)
		{      			
			GPIO_ResetBits(GPIOE,GPIO_Pin_0);
			GPIO_ResetBits(GPIOE,GPIO_Pin_1);
		}


	}
}   








void USART3_IRQHandler(void)
{
	char receive_data; 
 
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
    {
        receive_data = USART_ReceiveData(USART3); 
		USART_ClearITPendingBit(USART3, USART_IT_RXNE); // block the interrupt during data recieve
	 
		if( WhosData == CLEAR )
		{
  			if( receive_data == 0x33 )
			{
				WhosData = DATA_FINGER;
				GPIO_ResetBits(GPIOC,GPIO_Pin_13);
			}
			else
			if( receive_data == 0x55 )
			{
				WhosData = DATA_ARM;
			}
		}
		//if it's judged already
		else 
		{
			// process the finger
			if( WhosData == 1 )
			{
				DataFinger[++DataCount] =	receive_data;
				if(DataCount>=6)
				{
					DataCount = CLEAR;
					WhosData = CLEAR;
					GPIO_SetBits(GPIOC,GPIO_Pin_13);
				}
			}
			// survo motor to move the arm, not used
			/*
			if( WhosData == 2 )
			{
				DataArm[++DataCount] =	receive_data;
				if(DataCount>=4)
				{
					DataCount = CLEAR;
					WhosData = CLEAR;
				}
			}
			*/
			else
			{
				
			}
			
		}

        while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET ); 
        USART_ClearITPendingBit(USART3, USART_IT_RXNE); 
	}	
} 
