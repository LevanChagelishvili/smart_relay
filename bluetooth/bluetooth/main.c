//--------------------------------------------------------------------------------------------------------------------------------------------
/*
	For selecting synchronous or asynchronous modes use UMSELn register from control status register C (UCSRnC).
	Asynchronous normal mode U2Xn = 0, BAUD = fosc/16*(UBRRn + 1).
*/
//--------------------------------------------------------------------------------------------------------------------------------------------
#define F_CPU				8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define USART_BAUDRATE		9600
#define UBRR_VAL			(((F_CPU / (USART_BAUDRATE * 16UL))) - 1)
#define RELAY1				0
#define RELAY2				1
#define LED					0
#define LED_DELAY			100


void uart_init(uint8_t);
void transmit_bits(unsigned char);
unsigned char receive_bits();
void transmit_string(char*);
void led_blink();

int main(void)
{
    DDRC |= (1 << RELAY1) | (1 << RELAY2);
	DDRD |= (1 << 1);
	DDRD &= ~(1 << 0);
	PORTC |= (1 << RELAY1) | (1 << RELAY2);
	DDRB = (1 << 3) | (1 << LED);
	
	uart_init(UBRR_VAL);
	
	for(uint8_t i = 0; i < 4; i++)
	{
		led_blink();
	}
	

	
	char val;
	
    while (1) 
    {
		
		val = receive_bits();

		switch(val)
		{
			case '1':
				PORTC |= (1 << RELAY1);
				break;
			case '2':
				PORTC &= ~(1 << RELAY1);
				break;
			case '3':
				PORTC |= (1 << RELAY2);
				break;
			case '4':
				PORTC &= ~(1 << RELAY2);
				break;		
		}
		
    }
}


void uart_init(uint8_t ubrr)
{
	//Set baudrate
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	//Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); 
	//Set frame for 8bit data and 1 stop bit
	//Set 8 bit data UCSZ00 - 0; UCSZ01 - 1; UCSZ02 - 0;
	UCSR0C = (3 << UCSZ00);
	/*
	//Asynchronous USART
	UCSR0C &= ~(1 << UMSEL00);
	UCSR0C &= ~(1 << UMSEL01);
	//1 stop bit
	UCSR0C &= ~(1 << USBS0);
	//Parity mode disable
	UCSR0C &= ~(1 << UPM00);
	UCSR0C &= ~(1 << UPM01);	
	*/
}

void transmit_bits(unsigned char data_bits)
{
	//Wait for empty transmit buffer
	while(!(UCSR0A & (1 << UDRE0))){}
	//Put data into buffer
	UDR0 = data_bits;
}

unsigned char receive_bits()
{
	//Wait for data to be received
	while(!(UCSR0A & (1 << RXC0))){}
	//Get data
	return UDR0;
}
void transmit_string(char* str)
{
	while(*str > 0)
	{
		transmit_bits(*str++);
	}
}

void receive_string(char* str)
{
	while(*str > 0)
	{
		receive_bits(*str++);
	}
}

void led_blink()
{
	PORTB |= (1 << LED);
	_delay_ms(LED_DELAY);
	PORTB &= ~(1 << LED);
	_delay_ms(LED_DELAY);	
}