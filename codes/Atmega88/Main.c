#include <mega88.h>
#include <delay.h>
#include "ADC.h"
#include "Init.h"

#define TX1ON 0x02
#define TX2ON 0x04
#define DONE 0

#define Yes 1
#define No 0

unsigned char ismyTurn=1;

interrupt [TIM0_OVF] void timer_int0(void)
{
    ADC_TIMER++;
}


void Var_Init()
{
    channel = 0;
    ADC_flag = 0;
    ADC_TIMER = 0;  
    
    ismyTurn = 1;
}

void Transfer(unsigned char data )
{
    while ( !( UCSR0A & 0x20) );    //(1<<UDRE0)
    UDR0 = data;
}





void main1()
{
    All_Init();
    Var_Init(); 
    
    //collecting sensor data
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);

    
    PORTB = 0x02;  
    
    while(1)
    {
        //IF the other side Atmega port isn't HIGH
        if(PINB.2 == 0)
        {             
            if(ismyTurn==Yes)
            {
                //Turn this port High
                PORTB = TX1ON;  
                //send the data and after sending 1ms delay
                Transfer(0x33);
                Transfer(ADC_DATA[0]);
                Transfer(ADC_DATA[1]);
                Transfer(ADC_DATA[2]);
                Transfer(ADC_DATA[3]);
                Transfer(ADC_DATA[4]);   
                Transfer(0x99);     
                Transfer(0x99);
                ismyTurn=No;
                PORTB = DONE;
                delay_us(10);
            }
        }
        else
        {
            ismyTurn=Yes;
        }                                        
        ADC_Start(channel);
    }   

}


void main2()
{
    All_Init();
    Var_Init();
    
    //Collect the sensor data
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);
    ADC_Start(channel);
    delay_ms(2);

    
    while(1)
    {
        //IF the other side Atmega port isn't HIGH
        if(PINB.1 == 0)
        {                
            if(ismyTurn==Yes)
            {
                //Turn this port High
                PORTB = TX2ON;
                //send the data and after sending 1ms delay
                Transfer(0x55);
                Transfer(ADC_DATA[0]);
                Transfer(ADC_DATA[1]);
                Transfer(ADC_DATA[2]);
                Transfer(ADC_DATA[3]);
                Transfer(0x99);     
                Transfer(0x99);
                ismyTurn=No;     
                PORTB = DONE;
                delay_us(10);
            }
        }               
        else
        {
            ismyTurn=Yes;
        }                     
            
        ADC_Start(channel);
    }    

}

void main()
{
    main1();
}

