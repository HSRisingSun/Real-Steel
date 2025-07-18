#define ON 1
#define OFF 0
#define SET 1
#define CLEAR 0

#define LED1_ON 1
#define LED2_ON 2
#define LED3_ON 4
#define LED_OFF 7

//BAUDRATE. UBRR in 8MHz
#define BAUD_115200 3
#define BAUD_19200 25
#define BAUD_9600 51


void ADC_Init(void)
{
	ADMUX |= 0b01000000;
	ADCSRA |= 0b10000000;
}

void PORT_Init(void)
{
	DDRB |= 0b11111011;
	DDRC |= 0b00000000;
	DDRD |= 0b00000010;

	PORTC |= 0b11111111;
	PORTB = CLEAR;
	PORTD = CLEAR;

}

void USART_Init(void)
{

	//baud rate = (115200.UBRR -> 3) ,(9600.UBRR -> 51)
	UBRR0L = BAUD_9600;

	//USART status register
	UCSR0A |= 0b00000000;
	//Receiver, transmitter enable
	UCSR0B |= 0b00011000;
	//Asynchronous, No parity, 1stopbit, char size 8bit
	UCSR0C |= 0b00000110;

}

void TIMER0_Init(void)
{
	//pre-scaler 8. 1us
	TCCR0B |= 0b00000010;
	//overflow interrupt enable
	TIMSK0 |= 0x01;
	TCNT0 = 0;
}

void All_Init(void)
{
	ADC_Init();
	PORT_Init();
	USART_Init();
	TIMER0_Init();
	SREG |= 0x80;
}
