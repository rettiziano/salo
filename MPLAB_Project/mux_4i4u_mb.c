#include <16F628A.h>
#include "mux_4i4u_mb.h"
#include "..\modbus_library\spif.c"

/****************************************************************************************/
unsigned int16 reg[2];		

volatile unsigned int8 flag_1ms = 0;	// attivo nell'interrupt

#define LOCK 	1
#define	UNLOCK 	0

unsigned int8 lockGame = UNLOCK;				// blocca il gioco
unsigned int8 buttonStatus = 0;
	
#define DO_ADDR	0
#define DI_ADDR 1     					     				    					 				     				  					     		     					   

#define MAX_BUTTONS	4					// tasti attivi
#define MAX_DEBOUNCE 25					// debounce in ms

struct _button_							
{		
	unsigned int8 debounce;
	unsigned int8 set;
};

struct _button_ button[MAX_BUTTONS];	// struttura tasti

void RandomWins(void);
void Player1wins(void);
void Player2wins(void);
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

	column_bit++;
	if(column_bit)
	{// gestione delle colonne della matrice 2 x 2, prima colonna 
		if(input(DI0_2)) 
		{		
			if(button[0].debounce)
			{
				button[0].debounce--;

				if(button[0].debounce == 0)
				{
					button[0].set = 1;
					bit_set(buttonStatus,0);		// tasto 0 premuto
				}
			}
		} 
		else 
		{		
			bit_clear(buttonStatus,0);	// tasto 0 rilastiato
			button[0].debounce = MAX_DEBOUNCE;
		}

		if(input(DI1_3)) 
		{
			if(button[1].debounce)
			{
				button[1].debounce--;

				if(button[1].debounce == 0)
				{
					button[1].set = 1;
					bit_set(buttonStatus,1);		// tasto 1 premuto
				}
			}
 		}
		else 
		{
			bit_clear(buttonStatus,1);	// tasto 1 rialsciato
			button[1].debounce = MAX_DEBOUNCE;
		}

		output_low(COL);
	}// gestione delle colonne della matrice 2 x 2, prima colonna
	else
	{// gestione delle colonne della matrice 2 x 2, seconda colonna
		if(input(DI0_2)) 
		{
			if(button[2].debounce)
			{
				button[2].debounce--;

				if(button[2].debounce == 0)
				{
					button[2].set = 1;
					bit_set(buttonStatus,2); 		// tasto 2 premuto
				}
			}
		}
		else 
		{
			bit_clear(buttonStatus,2);
			button[2].debounce = MAX_DEBOUNCE;
		}		

		if(input(DI1_3)) 
		{
			if(button[3].debounce)
			{
				button[3].debounce--;

				if(button[3].debounce == 0)
				{
					button[3].set = 1;
					bit_set(buttonStatus,3); 		// tasto 3 premuto
				}
			}
		}
		else 
		{
			bit_clear(buttonStatus,3);
			button[3].debounce = MAX_DEBOUNCE;
		}	

		output_high(COL);
	}// gestione delle colonne della matrice 2 x 2, seconda colonna
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

	flag_1ms = 1;
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
	unsigned int8 i=0;
	
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

	for(i=0;i<MAX_BUTTONS;i++)
	{	
		button[i].debounce = MAX_DEBOUNCE;
		button[i].set = 0;
	}
	
	for(;;)
	{// main loop
		restart_wdt();

		if(flag_1ms)
		{
			read_DI();		// leggo gli incgressi
			coilOutput();	// scrivo le uscite
			flag_1ms = 0;
		}
		
		// gestisco la logica del gioco
		if ((button[0].set != 0) && (button[1].set != 0))
		{	// hanno premuto contemporaneamente. 
			if (lockGame == UNLOCK)
			{	// chi vincera?
  				RandomWins();
  				lockGame=LOCK; 		// fine dei giochi
			}
		}

		if ((button[0].set != 0) && button[1].set == 0)
		{ // ha vinto il player 1
			if (lockGame == UNLOCK)
			{	
  				Player1wins();
  				lockGame=LOCK; 		// fine dei giochi
			}		
		}
		
		if ((button[1].set != 0) && button[0].set == 0)
		{ // ha vinto il player 2
			if (lockGame == UNLOCK)
			{	
  				Player2wins();
  				lockGame=LOCK; 		// fine dei giochi
			}		
		}

		if(button[2].set != 0)
		{
			// riarmare il tutto
			if ((buttonStatus & 0x07) == 0)
			{	// tutti i pulsanti sono rilasciati
				lockGame=UNLOCK;
				button[0].set = 0;
				button[1].set = 0;
				button[2].set = 0;
			}
		}
	}// main loop
}//////////////////////////////  main ////////////////////	



void RandomWins(void)
{
	unsigned int8 value;
	printf("[RandomWins]\n");

	value=get_timer0();
	
	if(value & BIT0)
	{
		Player1wins();
	}
	else
	{
		Player2wins();
	}
}
	
	
void Player1wins(void)
{
	printf("[Player1wins]\n");
}
	
void Player2wins(void)
{
	printf("[Player2wins]\n");
}
