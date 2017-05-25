#define PIN_SPIF PIN_A3
/*******************************************************************************/

#ifndef BIT0
#define BIT0    1
#define BIT1    2
#define BIT2    4
#define BIT3    8
#define BIT4    16
#define BIT5    32
#define BIT6    64
#define BIT7    128
#define BIT8    256
#define BIT9    512
#define BIT10   1024
#define BIT11   2048
#define BIT12   4096
#define BIT13   8192
#define BIT14   16384
#define BIT15   32768
#endif


void spif_on (void) {
	output_bit(PIN_SPIF,1);
}

void spif_not (void) {
	output_bit (PIN_SPIF,~input(PIN_SPIF));
}

void spif_idle (void){
	output_bit (PIN_SPIF,input(PIN_SPIF));
}


void spif_off (void) {
	output_bit(PIN_SPIF,0);
}
                           


void spif_n8 (unsigned char enne)
{
	unsigned char n=8;
	
	spif_off();
	spif_off();
	spif_off();
	spif_off();
	
	while (n) 
	{
		spif_on();
		if (enne & BIT7)
		{
		 	spif_on();
		 	spif_on();
		 	spif_on();
		 	spif_on();
		}
		spif_off();
		enne <<=1;
		n--;
	}
	spif_off();
	spif_off();
	spif_off();
}


void spif_n16 (	unsigned int16 enne)
{
	unsigned char n=8;
	spif_off();
	spif_off();
	spif_off();
	spif_off();
	
	while (n) 
	{
		spif_on();
		if (enne & 0x8000)
		{
		 	spif_on();
		 	spif_on();
		 	spif_on();
		 	spif_on();
		}
		spif_off();
		enne <<=1;
		n--;
	}
	spif_off();
	n = 8;
	while (n) 
	{
		spif_on();
		if (enne & 0x8000)
		{
		 	spif_on();
		 	spif_on();
		 	spif_on();
		 	spif_on();
		}
		spif_off();
		enne <<=1;
		n--;
	}
	spif_off();
	spif_off();
	spif_off();
}



