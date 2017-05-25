/*
 * IDE: MPLAB           8.92.00.00    Certified
 * Compiler: CCS C 	4.120/4.057
*/

#case 

#include <16F648A.h>
#include "project.h"
#include "spif.c"

/****************************************************************************************/
volatile unsigned int8 flag_1ms = 0;	// attivo nell'interrupt
volatile unsigned int8 flag_100ms = 0;
volatile unsigned int8 flag_1s = 0;

#define	DIVISORE_SECONDI 1000

unsigned int16 I_O_EXCH(unsigned int16 dataIn);

/****************************************************************************************\
void coilOutput(void)		// reimposta le uscite digitali
{
	output_bit(DO0,bit_test(outputStatus,0));
	output_bit(DO1,bit_test(outputStatus,1));
	output_bit(DO2,bit_test(outputStatus,2));
	output_bit(DO3,bit_test(outputStatus,3));
}
\*****************************************************************************************/



/******************************************************************************************\
void coilReset(void)		// resetta le uscite digitali
{
	output_bit(DO0,0);
	output_bit(DO1,0);
	output_bit(DO2,0);
	output_bit(DO3,0);

	outputStatus = 0;
}
\******************************************************************************************/

#INT_TIMER0
void timer0_isr()
{ // scocca ad 1ms
	static unsigned int8 divisore100ms = 0;
	static unsigned int16 divisoreSecondi = 0;
	
	setup_timer_0(T0_DIV_16);
	set_timer0(28);

	flag_1ms = 1;
	
	divisore100ms++;
	if(divisore100ms == 100){
		divisore100ms = 0;
		flag_100ms = 1;
	}

	divisoreSecondi++; 
	if (divisoreSecondi == DIVISORE_SECONDI) { // un secondo
		divisoreSecondi = 0;
		flag_1s = 1;		
	} // if (divisoreSecondi == 0)

	spif_n8(1);
}

/******************************************************************************************\
unsigned char readAddr(void)
{
	unsigned char hardwareAddress = 0;	
	hardwareAddress |= input(ADDRESS_PIN_1)  << 2;   // Mas Sig. Bit
	hardwareAddress |= input(ADDRESS_PIN_2) << 1;   //
	hardwareAddress |= input(ADDRESS_PIN_3)  << 0;   // Less Sig. Bit
	// hardwareAddress += HW_ADDR_OFFSET; //0x18

	return hardwareAddress;	// 0 0 0 0x18 = 24
}
\******************************************************************************************/


unsigned int16 I_O_EXCH(unsigned int16 dataIn)
{
	unsigned int8 i;
	unsigned int16 dataOut = 0;
	unsigned int8 position;	// importante partire dall'ultima posizione

		// seriale/parallelo abilito hc165	
	output_low(PIN_SP);
	delay_us(10); // aspetta
	output_high(PIN_SP);
	
	// questi finiscono nel secondo STP U6
	for(i=0; i<16;i++) 
	{// scrive 16 bit sullo shift register
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_STP_DATA, ((dataIn & BIT15) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		output_high(PIN_STP_CK);	// clock salita
		delay_us(10); // aspetta
		
		position = 15-i; // parte daall'ultima posizione e decrementa
		
		if(input_state(PIN_READ)) {	// legge il valore del pin di ingresso, se 1...
			bit_set(dataOut,position); // setta il bit nella posizione indicata
		} else {
			bit_clear(dataOut,position); // resetta il bit nella posizione indicata
		}	
		
		output_low(PIN_STP_CK);	// clock discesa
		
		dataIn <<= 1;	// RoL Circular rotation left
		delay_us(10); // aspetta
	}

	// latch enable	
	output_high(PIN_STP_LE);
	delay_us(10); // aspetta
	output_low(PIN_STP_LE);
	

	return dataOut;
	
}


void main()
{//////////////////////////////  main ////////////////////	
	unsigned int16 data = 1;
	unsigned int16 retVal = 0;
	
	#use fast_io(A)
	#use fast_io(B)
	
	setup_timer_0(T0_DIV_16);
	// setup_wdt(WDT_2304MS);	

	setup_timer_1(T1_INTERNAL); 
	setup_ccp1(CCP_OFF);
	setup_comparator(NC_NC_NC_NC);
	
	set_tris_a(0b00000000);
	set_tris_b(0b00000001);
	set_timer0(28);
	
	enable_interrupts(GLOBAL);
	enable_interrupts(INT_TIMER0);
	
	
	for(;;)
	{// main loop
		// restart_wdt();

		if (flag_1s) { // e' passato 1 secondo
			flag_1s = 0;
			/* 
			var++; 
			if (var == 10s) { 
				// fai qualcosa 
				var = 0;
			}
			*/
		}	
		
		if(flag_100ms) { // sono passati 100ms
			flag_100ms = 0;
			
//			data = I_O_EXCH;
			data = 0x55ff;
			
			retVal = I_O_EXCH(data);
//			data <<=1;
			
			spif_n16(retVal);
		}
			

		if(flag_1ms) { // e' passato 1ms
			flag_1ms = 0;
		}

	}// main loop
}//////////////////////////////  main ////////////////////	
