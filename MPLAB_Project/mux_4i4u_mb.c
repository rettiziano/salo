#case 

#include <16F628A.h>
#include "mux_4i4u_mb.h"
#include "..\modbus_library\spif.c"

/****************************************************************************************/
unsigned int8 outputStatus = 0;		

volatile unsigned int8 flag_1ms = 0;	// attivo nell'interrupt

#define LOCK 	1
#define	UNLOCK 	0

#define	RELE0	BIT0
#define	RELE1	BIT1
#define	RELE2	BIT2
#define	RELE3	BIT3

#define	RELE_0_ON	outputStatus |= RELE0
#define	RELE_0_OFF	outputStatus &= ~RELE0 

#define	RELE_1_ON	outputStatus |= RELE1
#define	RELE_1_OFF	outputStatus &= ~RELE1

#define	RELE_3_ON	outputStatus |= RELE3
#define	RELE_3_OFF	outputStatus &= ~RELE3
// #define	RELE_3_TOGGLE	outputStatus ^= RELE3

// #define LED_HEARTBEAT_ON output_bit(PIN_B1,0)
// #define LED_HEARTBEAT_TOGGLE output_toggle(PIN_B1)

#define	TIME_HEARTBEAT	250

unsigned int8 lockGame = UNLOCK;		// libera il gioco
unsigned int8 buttonStatus = 0;

volatile unsigned int16 timeoutLampeggi[2] = {0,0};
volatile unsigned int16 timeoutSirena = 0;		// tempo attivazione sirena		
volatile unsigned int16 heartBeatTimer = 0;

#define MAX_TIME_SIRENA_ATTIVA 	1		// in secondi
#define MAX_LAMPEGGI			2000
	
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
void sirenaOn(void);
void sirenaOff(void);
void lampeggiante(unsigned int8 vincitore);
void HeartBeat(unsigned int8 status);
void LockGame(void);
/****************************************************************************************/

void coilOutput(void)		// reimposta le uscite digitali
{
	output_bit(DO0,bit_test(outputStatus,0));
	output_bit(DO1,bit_test(outputStatus,1));
	output_bit(DO2,bit_test(outputStatus,2));
	output_bit(DO3,bit_test(outputStatus,3));
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

	outputStatus = 0;
}
/******************************************************************************************/

#INT_TIMER0
void timer0_isr()
{ // scocca ad 1ms
	setup_timer_0(T0_DIV_16);
	set_timer0(28);

	flag_1ms = 1;
	
	if (timeoutSirena)
	{
		timeoutSirena--;
		if (timeoutSirena == 0) sirenaOff();
	}
	
	if(timeoutLampeggi[0])
	{
		timeoutLampeggi[0]--;
		
		switch (timeoutLampeggi[0])
		{
			case 0:
			case 500:
			case 1000:
			case 1500:
				RELE_0_ON;
			break;
			
			case 250:
			case 750:
			case 1250:
			case 1750:
				RELE_0_OFF;
			break;
		}
	}
	
	if(timeoutLampeggi[1])
	{
		timeoutLampeggi[1]--;
		
		switch (timeoutLampeggi[1])
		{
			case 0:
			case 500:
			case 1000:
			case 1500:
				RELE_1_ON;
			break;
			
			case 250:
			case 750:
			case 1250:
			case 1750:
				RELE_1_OFF;
			break;
		}
	}
	
	if(heartBeatTimer)
	{
		heartBeatTimer--;
		if (heartBeatTimer == 0)
		{
			heartBeatTimer = TIME_HEARTBEAT;
//			LED_HEARTBEAT_TOGGLE;
		}
	}
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
	
	coilReset();

	for(i=0;i<MAX_BUTTONS;i++)
	{	
		button[i].debounce = MAX_DEBOUNCE;
		button[i].set = 0;
	}
	
	lockGame = UNLOCK;		// blocca il gioco
	RELE_3_ON;
	
	HeartBeat(1);
	
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
  				LockGame(); 		// fine dei giochi
			}
		}

		if ((button[0].set != 0) && button[1].set == 0)
		{ // ha vinto il player 1
			if (lockGame == UNLOCK)
			{	
  				Player1wins();
  				LockGame(); 		// fine dei giochi
			}		
		}
		
		if ((button[1].set != 0) && button[0].set == 0)
		{ // ha vinto il player 2
			if (lockGame == UNLOCK)
			{	
  				Player2wins();
  				LockGame(); 		// fine dei giochi
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
				RELE_0_OFF;
				RELE_1_OFF;
				sirenaOff();
				HeartBeat(1);
			}
		}
	}// main loop
}//////////////////////////////  main ////////////////////	



void RandomWins(void)
{
	unsigned int8 value;
//	printf("[RandomWins]\n");

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
//	printf("[Player1wins]\n");
	sirenaOn(); 	// tira il rele 3 per 2 secondi
	lampeggiante(1);
}
	
void Player2wins(void)
{
//	printf("[Player2wins]\n");
	sirenaOn(); 	// tira il rele 3 per 2 secondi
	lampeggiante(2);
}


void sirenaOn(void)
{
	outputStatus |= RELE2; // tira il rele 3 e fa partire il timeout di disattivazione
	timeoutSirena = 1000 * MAX_TIME_SIRENA_ATTIVA;
	
}

void sirenaOff(void)
{
	outputStatus &= ~RELE2; // molla il rele 3
}

void lampeggiante(unsigned int8 vincitore)
{
	switch(vincitore)
	{
		case 1:
		{
			outputStatus |= RELE0; // tira il rele 0 e fa partire il timeout di disattivazione
			timeoutLampeggi[0] = MAX_LAMPEGGI;
		}
		break;
		
		case 2:
		{
			outputStatus |= RELE1; // tira il rele 1 e fa partire il timeout di disattivazione
			timeoutLampeggi[1] = MAX_LAMPEGGI;
		}
		break;
		
		default:
		break;
	}
}

void HeartBeat(unsigned int8 status)
{
	if(status == 0)
	{
		heartBeatTimer = 0;
		RELE_3_OFF;
	}
	else
	{
		heartBeatTimer = TIME_HEARTBEAT;
		RELE_3_ON;
	}
	
//	LED_HEARTBEAT_ON;
	
}

void LockGame(void)
{
	lockGame=LOCK; 		// fine dei giochi
	HeartBeat(0);
}