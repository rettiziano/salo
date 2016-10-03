#include <16F628A.h>
#include "mux_4i4u_mb.h"
#include "..\modbus_library\modbus_lib.c"
#include "..\modbus_library\generic_func_lib.c"
#include "..\modbus_library\spif.c"


#define MODBUS_MUX_4I4U_MB_VER $Revision: 1.4 $
#define MODBUS_MUX_4I4U_MB_DATE $Date: 2006/11/27 10:11:40 $
/****************************************************************************************/
// Codifica Tag Firmware Release = X.Y

// TAG_FW_RELEASE = bit15 ... bit0

// X = bit15...bit8     Y = bit7 ...bit0

#define TAG_FW_RELEASE 256  // =  1.0
/****************************************************************************************/
unsigned char backin[4];
unsigned char counter;    	// contatore utilizzato nella routine di interrupt
unsigned char dev_address;
unsigned int16 temp;
unsigned int16 coda_uscite;
unsigned char prima, adesso;
bit col_bit;				     					     				    					 				     				  					     		     					   
/****************************************************************************************/

void coilOutput(void)		// reimposta le uscite digitali
{
	output_bit(DO0,bit_test(reg[DO_ADDR],0));
	output_bit(DO1,bit_test(reg[DO_ADDR],1));
	output_bit(DO2,bit_test(reg[DO_ADDR],2));
	output_bit(DO3,bit_test(reg[DO_ADDR],3));

	adesso = (*0x05 & 0x0f);
	if (adesso != prima)
	{
		write_eeprom (21, adesso);
		prima = adesso;
	}
}
/*****************************************************************************************/

void read_DI(void)
{
	col_bit++;
	if(col_bit)
	{// gestione delle colonne della matrice 2 x 2, prima colonna 
		if(input(DI0_2)) bit_set(temp,0); else bit_clear(temp,0);
		if(input(DI1_3)) bit_set(temp,1); else bit_clear(temp,1);	
		output_low(COL);
	}// gestione delle colonne della matrice 2 x 2, prima colonna
	else
	{// gestione delle colonne della matrice 2 x 2, seconda colonna
		if(input(DI0_2)) bit_set(temp,2); else bit_clear(temp,2);
		if(input(DI1_3)) bit_set(temp,3); else bit_clear(temp,3);
		output_high(COL);
	}// gestione delle colonne della matrice 2 x 2, seconda colonna
	
	if (bit_test(reg[DO_ADDR],8) != 0)
	{// la configurazione dice che gli ingressi vanno gestiti localmente
		unsigned char zz;
		for (zz = 0; zz != 4; zz++)
		{// per quanti sono gli ingressi fai
			unsigned char readin;
			readin = ((temp & (BIT0<<ZZ)) != 0);
			
			if (readin)
			{// input attivo, ma prima come stava?
				if (backin[zz] < 4)
				{// antirimbalzo in corso
					backin[zz]++;
					if (backin[zz] == 4)
					{// debounce time raggiunto
						coda_uscite = (0x3FF / 24);
						reg[DO_ADDR] ^= (BIT0 << zz);
					}// debounce time raggiunto
				}// antirimbalzo in corso
			}// input attivo, ma prima come stava?
			else
			{// input disattivo, riarma debounce
				backin[zz] = 0;
			}// input disattivo, riarma debounce
		}// per quanti sono gli ingressi fai
	
		reg[DI_ADDR] = reg[DO_ADDR];
	}// la configurazione dice che gli ingressi vanno gestiti localmente
	else
	{// la configurazione dice che sono delle normali 4i4o
		reg[DI_ADDR] |= temp;
	}// la configurazione dice che sono delle normali 4i4o
}




/******************************************************************************************/
void coilReset(void)		// resetta le uscite digitali
{
	output_bit(DO0,0);
	output_bit(DO1,0);
	output_bit(DO2,0);
	output_bit(DO3,0);
	
	write_eeprom(0x02,0);
}
/******************************************************************************************/

#int_rda                            	// salta cada vez que recibe los datos disponibles de la rs232
void serial_isr()
{//////////////////////// serial //////////////////////////////
	flagsm |= BFM_NEW_FRAME;
	flagsm &= ~BFM_PARITY_ERR;
	counter = 0;
	for(;;)
	{// loop di ricezione seriale, esce con dei break                					// espera mientras que stop cambie
		while(!kbhit()) 				// kbhit() ritorna 1 quando un carattere e' stato ricevuto
		if(TMR1IF)						// controlla se e' scaduto il tempo massimo entro cui ricevere il successivo carattere cioe' t15
		{// tempo scaduto, frame terminato
			set_timer1(count_35_isr);	// se e' trascorso t15 esco da serial_isr() e imposto il timer a count_35_isr = t35-t15
			TMR1IF = 0;
			return;
		}
		
		set_timer1(count_15);						// se e' arrivato un nuovo carattere imposto il timer a t15
		TMR1IF = 0;                    	// getc pone o recibe lo que hay en reg. buffer
		buffer[counter++] = getc(); 		// incrementa el contador

	#ifndef ICU2000_PROTOCOL
		if(RS232_ERRORS & BIT0)				// parity error check
			flagsm |= BFM_PARITY_ERR;
	#endif
		if (counter > BUFFER_SIZE)
		{
			reg[OVERRUN_COUNT_ADDR] += 1;	
			set_timer1(count_35);			// se il buffer e' pieno esco da serial_isr() e imposto il timer a count_35
			TMR1IF = 0;
			return;
		}
	}// loop di ricezione seriale, esce con dei break                					// espera mientras que stop cambie
}//////////////////////// serial //////////////////////////////

unsigned char readAddr(void)
{
	unsigned char tmp = 0;	
	tmp |= input(FIRST_PIN)  << 2;   // Mas Sig. Bit
	tmp |= input(SECOND_PIN) << 1;   //
	tmp |= input(THIRD_PIN)  << 0;   // Less Sig. Bit
	tmp += HW_ADDR_OFFSET; //0x18
	return tmp;	// 0 0 0 0x18 = 24
}




/******************************************************************************************/

void main()
{//////////////////////////////  main ////////////////////	
	#use fast_io(A)
	#use fast_io(B)
	
	setup_timer_0(RTCC_INTERNAL);
	setup_wdt(WDT_2304MS);		
	setup_timer_1(T1_INTERNAL); 
	setup_ccp1(CCP_OFF);
	setup_comparator(NC_NC_NC_NC);
	
	set_tris_a(0b11100000);
	set_tris_b(0b11110111);
	
	enable_interrupts(GLOBAL);
	enable_interrupts(INT_RDA);
	disable_interrupts(INT_TIMER1);
	
	coda_uscite = 0;
	if (restart_cause() == NORMAL_POWER_UP)
	{
		for(i=0;i<BUFFER_SIZE;i++)
			buffer[i]=0;
		for(i=0;i<REG_SIZE;i++)
			reg[i]=0;		
	}		
	
	adesso = prima = read_eeprom (21);
	*0x05 = adesso;

	if (read_eeprom(0x00)==0xFF) 	// Eeprom never written: 0xFF after programming -> chip appena riprogrammato
	{ 																// Imposto la configurazione di default
		setDefaultUart();		
		coilReset();
		write_eeprom(reg[UART_ADDR],0x01);
		write_eeprom(0x00,0);   // Write_eeprom(addr,value) = 0x00		// indico che al prossimo power-up il chip e' gia' stato programmato 
	}																// e quindi leggo la configurazione precedentemente salvata in eeprom
	else						// Read configuration from eeprom 
	{
		reg[UART_ADDR] = read_eeprom(0x01);
		configUart();
	}
	
	reg[FW_RELEASE_ADDR] = TAG_FW_RELEASE;		
	flagsm = 0;
	set_timer1(count_35);
	
	for(;;)
	{// main loop
		dev_address = readAddr();
		if(flagsm & BFM_NEW_FRAME)
		{// dato seriale arrivato	
			if (coda_uscite) coda_uscite--;
			read_DI();
			coilOutput();
			
			if((flagsm & BFM_PARITY_ERR) || (!CRC16(counter - 2,1)) || (counter < 3)) //CRC16 Error
			{// errori seriali
				reg[ERR_COUNT_ADDR] += 1;
			}// errori seriali
			else
			{// non ci sono errori sulla seriale
				reg[MESS_COUNT_ADDR] += 1;
				if((buffer[0] == dev_address) || (!buffer[0]))
				{// è l'indirizzo modbus giusto
					reg[SLAVE_COUNT_ADDR] += 1;  // counts n°messages detected on the bus addressed to the device and processed
					if(!buffer[0])	
					{// INDIRIZZO MODBUS == 0, BROADCAST, non rispondera al comando
						reg[SLAVE_NO_RESP_ADDR] += 1;
					}// INDIRIZZO MODBUS == 0, BROADCAST, non rispondera al comando
					
					function_call();
					coilOutput();
					
					if(flagsm & BFM_WRITE_REG)
					{
						if(reg[UART_ADDR] != read_eeprom(0x01))
						{
							configUart();
							write_eeprom(0x01,reg[UART_ADDR]);
						}
					}
				
					if(flagsm & BFM_RESTART)
					{
						clearCounters();
						configUart();
					}
				}// è l'indirizzo modbus giusto
			}// non ci sono errori sulla seriale
			flagsm &= BFM_LISTEN;		//flags reset except LISTEN_FLAG 	
		}// dato seriale arrivato					
	}// main loop
}//////////////////////////////  main ////////////////////	

