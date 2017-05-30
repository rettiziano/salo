/*
 * IDE: MPLAB           8.92.00.00    Certified
 * Compiler: CCS C 	4.120/4.057
*/

#case 

#include <16F648A.h>
#include "project.h"

// unsigned int16 data = 0;

/****************************************************************************************/
// assegnare SOLO QUI il pin della spif
#define PIN_SPIF PIN_A3
#include "spif.c"
/****************************************************************************************/

volatile unsigned int8 flag_1ms = 0;	// attivo nell'interrupt
volatile unsigned int8 flag_100ms = 0;
volatile unsigned int8 flag_1s = 0;

#define	DIVISORE_SECONDI 1000

unsigned int16 I_O_EXCH(unsigned int16 output_data);
unsigned int8 scriviLettera(unsigned int8 lettera);
unsigned int8 scriviNumero(unsigned int8 numero);

void MainProgram(void);

static unsigned int16 dataOUT = 0;	
	
/****************************************************************************************\
void coilOutput(void)		// reimposta le uscite digitali
{
	output_bit(DO0,bit_test(outputStatus,0));
	output_bit(DO1,bit_test(outputStatus,1));
	output_bit(DO2,bit_test(outputStatus,2));
	output_bit(DO3,bit_test(outputStatus,3));
}
\*****************************************************************************************/

#define PL(x) scriviLettera(x)
#define PN(x) scriviNumero(x)

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


static const unsigned int8 numeri[] = {
	DISPLAY_0, // visualizza sul diplay 0
	DISPLAY_1, // visualizza sul diplay 1
	DISPLAY_2, // visualizza sul diplay 2
	DISPLAY_3, // visualizza sul diplay 3
	DISPLAY_4, // visualizza sul diplay 4
	DISPLAY_5, // visualizza sul diplay 5
	DISPLAY_6, // visualizza sul diplay 6
	DISPLAY_7, // visualizza sul diplay 7
	DISPLAY_8, // visualizza sul diplay 8
	DISPLAY_9, // visualizza sul diplay 9
};

static const unsigned int8 lettere[] = {
	DISPLAY_A, // visualizza sul diplay A
	DISPLAY_B, // visualizza sul diplay B
	DISPLAY_C, // visualizza sul diplay C
	DISPLAY_D, // visualizza sul diplay D
	DISPLAY_E, // visualizza sul diplay E
	DISPLAY_F, // visualizza sul diplay F
	DISPLAY_P, // visualizza sul diplay P
	DISPLAY_I, // visualizza sul diplay I
	DISPLAY_N, // visualizza sul diplay n
	DISPLAY_T, // visualizza sul diplay t
	DISPLAY_R, // visualizza sul diplay r
	DISPLAY_S, // visualizza sul diplay r
	DISPLAY_L, // visualizza sul diplay L
};

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


unsigned int16 I_O_EXCH(unsigned int16 output_data)
{
	unsigned int8 i;
	unsigned int16 input_data = 0;
//	unsigned int8 position;	// importante partire dall'ultima posizione
	static const unsigned int8 lastPosition = 15;

	output_low(PIN_SER_PAR);	// leggo l'ingresso d hc165
	delay_us(10); // aspetta
	output_high(PIN_SER_PAR);
	
	// questi finiscono nel secondo STP U6
	for(i=0; i<=lastPosition;i++) 
	{// scrive e legge 16 bit sullo shift register
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_STP_DATA, ((output_data & BIT15) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		input_data |= (input_state(PIN_READ) & BIT0);
		// input_data |= (bit_test (data, LAST_POSITION-i));
//		spif_n8(i);
//		spif_n16(input_data);
		
		if(i!= lastPosition) {
			// non devo spostare a sinistra la posizione della variabile di uscita nell-ultimo giro.
			input_data<<=1; // RoL Circular rotation left
		}
		
//		spif_n16(input_data);
		
		output_data <<= 1;	// RoL Circular rotation left
		delay_us(10); // aspetta
		
		output_high(PIN_STP_CK);	// clock salita
		delay_us(10); // aspetta
		output_low(PIN_STP_CK);		// clock discesa			
	}

	output_high(PIN_STP_LE);	// latch enable	faccio uscire sul 4094
	delay_us(10); // aspetta
	output_low(PIN_STP_LE);
	

	return input_data;
	
}
/////////////////////////////////////////////////////////////
unsigned int8 DISP(unsigned int8 out_disp_1, out_disp_2, out_disp_3, out_disp_4)
{
	unsigned int8 i;
    static const unsigned int8 lastPosition = 7;
		
	// spif_n8(0xd0);	
	output_low(PIN_A2);		// abilito il display
	delay_us(10); 			// aspetta
	

	output_high(PIN_A1);	// clock salita display 33
	delay_us(10); 			// aspetta
	output_low(PIN_A1);		// clock discesa display
	delay_us(10); 			// aspetta
	output_high(PIN_A1);	// clock salita display 34
	delay_us(10); 			// aspetta
	output_low(PIN_A1);		// clock discesa display
	delay_us(10);			// aspetta
	output_high(PIN_A1);	// clock salita display 35
	delay_us(10); 			// aspetta
	output_low(PIN_A1);		// clock discesa display 
	delay_us(10);			// aspetta
	
	output_high(PIN_A0);		// do due colpi di clock con il bit dati a uno
	delay_us(10); 			// aspetta
		
	output_high(PIN_A1);		// clock salita display 36
	delay_us(10); 			// aspetta
	output_low(PIN_A1);		// clock discesa display
	delay_us(10);			// aspetta
	
	output_low(PIN_A0);		// fine ciclo due colpi di clock
	delay_us(10); 			// aspetta		

	for(i=0; i<=lastPosition;i++) 
	{// scrive 8 bit sullo shift register del display
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_A0, ((out_disp_1 & BIT7) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		
		out_disp_1 <<= 1;	// RoL Circular rotation left
		delay_us(10); 		// aspetta
		
		output_high(PIN_A1);	// clock salita display
		delay_us(10); 		// aspetta
		output_low(PIN_A1);	// clock discesa display			
	}
	
	for(i=0; i<=lastPosition;i++) 
	{// scrive 8 bit sullo shift register del display
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_A0, ((out_disp_2 & BIT7) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		
		out_disp_2 <<= 1;	// RoL Circular rotation left
		delay_us(10); 		// aspetta
		
		output_high(PIN_A1);	// clock salita display
		delay_us(10); 		// aspetta
		output_low(PIN_A1);	// clock discesa display			
	}
	
	for(i=0; i<=lastPosition;i++) 
	{// scrive 8 bit sullo shift register del display
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_A0, ((out_disp_3 & BIT7) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		
		out_disp_3 <<= 1;	// RoL Circular rotation left
		delay_us(10); 		// aspetta
		
		output_high(PIN_A1);	// clock salita display
		delay_us(10); 		// aspetta
		output_low(PIN_A1);	// clock discesa display			
	}
	
	for(i=0; i<=lastPosition;i++) 
	{// scrive 8 bit sullo shift register del display
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_A0, ((out_disp_4 & BIT7) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		
		out_disp_4 <<= 1;	// RoL Circular rotation left
		delay_us(10); 		// aspetta
		
		output_high(PIN_A1);	// clock salita display
		delay_us(10); 		// aspetta
		output_low(PIN_A1);	// clock discesa display			
	}


	output_high(PIN_A2);	// disabilito il display
	

	return 0;
	
}
/////////////////////////////////////////////////////////////


void main()
{//////////////////////////////  main ////////////////////	

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
	
	// DISP (DISPLAY_OFF,DISPLAY_OFF,DISPLAY_OFF,DISPLAY_OFF);
	DISP (PL(_I),PL(_D),PL(_L),PL(_E));
	delay_ms(2000);
	
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
		
		if(flag_100ms) 
		{ // sono passati 100ms
			flag_100ms = 0;
//			data = 0b0000000000000001;
//			dataIN = I_O_EXCH(0b0101010101010101);
//			dataIN = I_O_EXCH(dataIN);
//			spif_n16(data);	
//			spif_n16(dataIN);
//			data = 0x00ff;			
			
//			data <<=1;
//////////////////////////////////////////////
			// DISP (PL(_P),PN(0),PN(1),PN(2));
		
			
//////////////////////////////////////////////			
		}

		if(flag_1ms) { // e' passato 1ms
			MainProgram();
			// spif_n8(1);
			flag_1ms = 0;
		}

	}// main loop
}//////////////////////////////  main ////////////////////	



unsigned int8 scriviLettera(unsigned int8 lettera)
{
	return 	lettere[lettera];
}	


unsigned int8 scriviNumero(unsigned int8 numero)
{
	return 	numeri[numero];
}	



void MainProgram(void)
{
	//------------------entro ogni ms --------------------------------------
	static unsigned int8 statoMacchina = ST_IDLE;	
	static unsigned int8 statoErrore = NO_ERROR;

	static unsigned int8 provenienza = 0;
	static unsigned int16 wait = 0;	
	
	unsigned int16 dataIN = 0;



	static unsigned int8 testSeq = 0;

// 	unsigned int16 retVal = 0;
	
	// dataIN = I_O_EXCH(dataOUT);
	//	spif_n16(dataIN);
	
	dataIN = I_O_EXCH(dataOUT);	// leggo anche i tasti
			
	// spif_n16(dataIN);

	if ((dataIN & SENSORI) == 0) 
	{	
		// DISP (PL(_P),PN(0),PN(0),PN(0));			
		statoMacchina = ST_ALARM_1;
		dataOUT = LUCE_ROSSA; //accendo solo la luce ROSSA del lampeggiante
		I_O_EXCH(dataOUT);	// scrivo l'usicta
		// spif_n16(dataIN);
		spif_n16(0xE001);
		DISP (PL(_E),PL(_R),PL(_R),PN(ERROR_1));			
		statoErrore = ERROR_1;
		break;
	} 

	if (dataIN & ARRESTO_STOP)
	{
		// DISP (PL(_P),PN(0),PN(0),PN(1));	
		if(statoMacchina != ST_ARRESTO) {
			provenienza = statoMacchina; // metto via
			// spif_n8(provenienza);
			statoMacchina = ST_ARRESTO;
			dataOUT |= LUCE_GIALLA; //accendo in piu la luce gialla del lampeggiante
			I_O_EXCH(dataOUT);	// leggo anche i tasti
			// spif_n16(dataIN);
			spif_n16(0xE002);
			DISP (PN(5),PL(_T),PN(0),PL(_P));
			DISP (PN(5),PL(_T),PN(0),PL(_P));
			statoErrore = ERROR_2;	
		}
		break;
	} else {
		if(statoMacchina == ST_ARRESTO) {
			if(provenienza == ST_WAIT) {
				statoMacchina = ST_WAIT;
				provenienza = ST_START;
			}
			// spif_n8(provenienza);
			if(dataOUT & LUCE_GIALLA) {
				dataOUT &= ~LUCE_GIALLA; //accendo in piu la luce gialla del lampeggiante
				I_O_EXCH(dataOUT);	// leggo anche i tasti
			}
			break;
		}
	}
	
	
	if (dataIN & MARCIA_START)
	{	
		if(statoMacchina == ST_IDLE) {
			statoMacchina = ST_START;
			DISP (PL(_S),PL(_T),PL(_R),PL(_T));	
			statoMacchina = ST_START;
			dataOUT = LUCE_VERDE; //si parte, luce verde
			I_O_EXCH(dataOUT);	// leggo anche i tasti
			// spif_n16(dataIN);
			spif_n16(0x5001);
		}
		break;
	} else {
		// sto fermo qua in attesa
		
	} 	
	
	
	switch(statoMacchina) //cicla tra un case e l'altro in funzione dello stato di avanzamento
	{
		case ST_IDLE:	// passo 0 qui dentro ciclo quando c'e' un arresto macchina 
		{ 
			
		}
		break;
		
		case ST_ALARM_1:
		{
			// sto qua fin che muoio per il momento

			dataIN = I_O_EXCH(dataOUT);	// leggo anche i tasti
			
			// spif_n16(dataIN);
			// spif_n16(0xEE01);
			
			if(dataIN & SWITCH_RESET) {
				// esco da qua premendo un reset.
				DISP (PL(_I),PL(_D),PL(_L),PL(_E));
				statoMacchina	= ST_IDLE;
				break;
			} else {
				// resto qua in attesa;
			}	
			
		}	
		break;
		
		
		case ST_ARRESTO:
		{
			// sto qua fin che muoio per il momento

			dataIN = I_O_EXCH(dataOUT);	// leggo anche i tasti
			
			// spif_n16(dataIN);
			// spif_n16(0xEE02);
			
			if(dataIN & SWITCH_RESET) {
				// esco da qua premendo un reset.
				DISP (PL(_I),PL(_D),PL(_L),PL(_E));
				statoMacchina	= ST_IDLE;
				break;
			} else {
				// resto qua in attesa;
			}		
		}	
		break;
		
		case ST_START:
		{
			// sto qua fin che muoio per il momento
			statoErrore = NO_ERROR; // tenere o buttare l'ultimo errore
			provenienza = ST_START;
			// ciclo qua dentro ogni ms
			spif_n8(testSeq);
			DISP (PN(testSeq),PN(testSeq),PN(testSeq),PN(testSeq));	
			wait = 1000;	// prima di tornare qua aspetto un tempo "wait"
			testSeq++;
			testSeq &= 7; //  si ferma a 7
			// spif_n8(testSeq);
			if (testSeq == 8)  {
				// quando arrivza a 8 va in allarme
				// provenienza = ST_ALARM_1; 
				// testSeq 
			}
				
			statoMacchina = ST_WAIT;
		}	
		break;
			
		case ST_WAIT:
		{
			if(wait) {
				wait--;
				if(wait == 0) {
					// esci.
					statoMacchina = provenienza; // torna indietro
				}		
			}
		}	
		break;	
			
		default:
		
			statoMacchina = 0;
			break;
	}
		
	//--------------------------------------------------------	
}	


