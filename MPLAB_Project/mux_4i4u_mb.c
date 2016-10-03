#include <16F628A.h>
#include "mux_4i4u_mb.h"
#include "..\modbus_library\spif.c"

/****************************************************************************************/
unsigned int16 reg[2];		

#define DO_ADDR	0
#define DI_ADDR 1     					     				    					 				     				  					     		     					   
/****************************************************************************************/

void coilOutput(void)		// reimposta le uscite digitali
{
	output_bit(DO0,bit_test(reg[DO_ADDR],0));
	output_bit(DO1,bit_test(reg[DO_ADDR],1));
	output_bit(DO2,bit_test(reg[DO_ADDR],2));
	output_bit(DO3,bit_test(reg[DO_ADDR],3));
}
/*****************************************************************************************/

void read_DI(void)
{
	static bit column_bit = 0;	
	static unsigned int8 tempButton;
	unsigned char channel;
	unsigned char readInput;	
	static unsigned char debounce[4];

	column_bit++;
	if(column_bit)
	{// gestione delle colonne della matrice 2 x 2, prima colonna 
		if(input(DI0_2)) bit_set(tempButton,0); else bit_clear(tempButton,0);
		if(input(DI1_3)) bit_set(tempButton,1); else bit_clear(tempButton,1);	
		output_low(COL);
	}// gestione delle colonne della matrice 2 x 2, prima colonna
	else
	{// gestione delle colonne della matrice 2 x 2, seconda colonna
		if(input(DI0_2)) bit_set(tempButton,2); else bit_clear(tempButton,2);
		if(input(DI1_3)) bit_set(tempButton,3); else bit_clear(tempButton,3);
		output_high(COL);
	}// gestione delle colonne della matrice 2 x 2, seconda colonna


	for (channel = 0; channel != 4; channel++)
	{// per quanti sono gli ingressi fai
		readInput = ((tempButton & (BIT0<<channel)) != 0);
		
		if (readInput)
		{// input attivo, ma prima come stava?
			if (debounce[channel] < 4)
			{// antirimbalzo in corso
				debounce[channel]++;
				if (debounce[channel] == 4)
				{// debounce time raggiunto
					reg[DO_ADDR] ^= (BIT0 << channel);
				}// debounce time raggiunto
			}// antirimbalzo in corso
		}// input attivo, ma prima come stava?
		else
		{// input disattivo, riarma debounce
			debounce[channel] = 0;
		}// input disattivo, riarma debounce
	}// per quanti sono gli ingressi fai

	reg[DI_ADDR] = reg[DO_ADDR];
}




/******************************************************************************************/
void coilReset(void)		// resetta le uscite digitali
{
	output_bit(DO0,0);
	output_bit(DO1,0);
	output_bit(DO2,0);
	output_bit(DO3,0);

	reg[DO_ADDR] = 0;
}
/******************************************************************************************/

#int_rda                            	// salta cada vez que recibe los datos disponibles de la rs232
void serial_isr()
{//////////////////////// serial //////////////////////////////
           					// espera mientras que stop cambie
}//////////////////////// serial //////////////////////////////

#INT_TIMER0
void timer0_isr()
{ // scocca ad 1ms
	set_timer0(28);
}



unsigned char readAddr(void)
{
	unsigned char hardwareAddress = 0;	
	hardwareAddress |= input(ADDRESS_PIN_1)  << 2;   // Mas Sig. Bit
	hardwareAddress |= input(ADDRESS_PIN_2) << 1;   //
	hardwareAddress |= input(ADDRESS_PIN_3)  << 0;   // Less Sig. Bit
	hardwareAddress += HW_ADDR_OFFSET; //0x18

	return hardwareAddress;	// 0 0 0 0x18 = 24
}




/******************************************************************************************/

void main()
{//////////////////////////////  main ////////////////////	
	#use fast_io(A)
	#use fast_io(B)
	
	setup_timer_0(T0_DIV_16);
	setup_wdt(WDT_2304MS);		
	setup_timer_1(T1_INTERNAL); 
	setup_ccp1(CCP_OFF);
	setup_comparator(NC_NC_NC_NC);
	
	set_tris_a(0b11100000);
	set_tris_b(0b11110111);
	set_timer0(28);
	
	enable_interrupts(GLOBAL);
	enable_interrupts(INT_TIMER0);
	
	reg[DO_ADDR] = 0;
	reg[DI_ADDR] = 0;

	coilReset();
	
	for(;;)
	{// main loop
		restart_wdt();
		read_DI();
		coilOutput();

	// value=get_timer0()   

	}// main loop
}//////////////////////////////  main ////////////////////	

