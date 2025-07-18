
/*
void Recive(void)
{
	//Never Never Never Forget.
}


void Transfer(unsigned char data)
{
	while(UDRE0);	// wait till the USART data register clears
	UDR0 = data;
}
*/
void Transfer(unsigned char data )
{
	while ( !( UCSR0A & 0x20) );	//(1<<UDRE0)
	UDR0 = data;
}
