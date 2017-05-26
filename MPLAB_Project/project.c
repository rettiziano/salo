/*
 * IDE: MPLAB           8.92.00.00    Certified
 * Compiler: CCS C 	4.120/4.057
*/

#case 

#include <16F648A.h>
#include "project.h"

#define PIN_SPIF PIN_A3
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
	static unsigned int16 divisore100ms = 0;
	static unsigned int16 divisoreSecondi = 0;
	
	setup_timer_0(T0_DIV_16);
	set_timer0(28);

	flag_1ms = 1;
	
	divisore100ms++;
	if(divisore100ms >= 100){
		divisore100ms = 0;
		flag_100ms = 1;
	}

	divisoreSecondi++; 
	if (divisoreSecondi >= DIVISORE_SECONDI) { // un secondo
		divisoreSecondi = 0;
		flag_1s = 1;		
	} // if (divisoreSecondi == 0)

	// spif_n8(1);
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


unsigned int16 I_O_EXCH(unsigned int16 dataIn = 0)
{
	unsigned int8 i;
	unsigned int16 dataOut = 0;
//	unsigned int8 position;	// importante partire dall'ultima posizione

			
	output_low(PIN_SP);	// leggo l'ingresso d hc165
	delay_us(10); // aspetta
	output_high(PIN_SP);
	
	// questi finiscono nel secondo STP U6
	for(i=0; i<16;i++) 
	{// scrive e legge 16 bit sullo shift register
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_STP_DATA, ((dataIn & BIT15) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		

		
		dataOut |= (input_state(PIN_READ) & BIT0);
		dataOut<<=1; // RoL Circular rotation left
		
		dataIn <<= 1;	// RoL Circular rotation left
		delay_us(10); // aspetta
		
		
		output_high(PIN_STP_CK);	// clock salita
		delay_us(10); // aspetta
		output_low(PIN_STP_CK);		// clock discesa			
	
	
	}

	
	output_high(PIN_STP_LE);	// latch enable	faccio uscire sul 4094
	delay_us(10); // aspetta
	output_low(PIN_STP_LE);
	

	return dataOut;
	
}


void main()
{//////////////////////////////  main ////////////////////	
	unsigned int16 data = 0, dataIN = 0, dataOUT = 0;
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
	
//	data = 0x55ff;
//	spif_n16(data);
//	retVal = I_O_EXCH(data);
	
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
			data = 0b0000000000000001;
			dataIN = I_O_EXCH;
			dataOUT = dataIN;
			dataIN = I_O_EXCH(data);
			spif_n16(dataIN);
//			data = 0x00ff;			
			
//			data <<=1;
			
			
		}
			

		if(flag_1ms) { // e' passato 1ms
			flag_1ms = 0;
		}

	}// main loop
}//////////////////////////////  main ////////////////////	
