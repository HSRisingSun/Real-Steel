
void USART_Transmit( unsigned int data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRnA & (1<<UDREn))) );
	
	/* Copy 9th bit to TXB8 */
	UCSRnB &= ~(1<<TXB8);
	if ( data & 0x0100 )
	UCSRnB |= (1<<TXB8);

	/* Put data into buffer, sends the data */
	UDRn = data;
}

unsigned char USART_Receive( void )
{
	/* Wait for data to be received */
	while ( !(UCSRnA & (1<<RXCn)) );
	
	/* Get and return received data from buffer */
	return UDRn;
}
