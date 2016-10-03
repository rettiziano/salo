/************************************************************************************/
/************************************************************************************/
/*********************       Parametri di Configurazione       **********************/
/************************************************************************************/
/************************************************************************************/

#define DEFAULT_BAUD_RATE		BAUD_RATE_3

#define BAUD_RATE_0				9600
#define BAUD_RATE_1				19200
#define BAUD_RATE_2				28800
#define BAUD_RATE_3				38400
#define BAUD_RATE_4				57600
#define BAUD_RATE_5				115200	

/************************************************************************************/

#define HW_ADDR_OFFSET		0x00	// see ..\mmaster\modbus\Peripheral_Hw_Addr.xls

#define FW_RELEASE_ADDR		0	// indirizzo in reg[] del Tag Firmware Release 				
#define UART_ADDR         7	// indirizzo in reg[] del registro di configurazione della UART
#define DO_ADDR						8
#define DI_ADDR						9

/********************* #defines used in ModBus_Lib.c **********************/

#define BUFFER_SIZE				30
#define REG_SIZE				10

#define NUM_DIG_INPUTS          16	// Multipli di 16  	
#define NUM_HOLD_REG 			LAST_HOLD_REG_ADDR - FIRST_HOLD_REG_ADDR + 1
#define NUM_INPUT_REG			7
#define NUM_COIL            	0
#define NUM_COUNTERS        	6
#define MIN_REG_VAL			   		0
#define MAX_REG_VAL         	65535
	
#define COIL_ADDR           	0	// dummy
#define DIG_INPUTS_ADDR				0	// dummy
#define INPUT_REG_ADDR				0	// indirizzo in reg[] del primo Input Register
#define FIRST_COUNTER_ADDR  	1	// indirizzo in reg[] del primo contatore
#define MESS_COUNT_ADDR     	1	// indirizzo in reg[] del contatore "Bus Message Count"	
#define ERR_COUNT_ADDR      	2	// indirizzo in reg[] del contatore "Bus Communication Error Count" 
#define EXC_COUNT_ADDR      	3	// indirizzo in reg[] del contatore "Slave Exception Error Count" 
#define SLAVE_COUNT_ADDR    	4	// indirizzo in reg[] del contatore "Slave Message Count" 
#define SLAVE_NO_RESP_ADDR    5	// indirizzo in reg[] del contatore "Slave No Response Count"
#define OVERRUN_COUNT_ADDR		6	// indirizzo in reg[] del contatore "Bus Character Overrun" 
#define FIRST_HOLD_REG_ADDR   7	// indirizzo in reg[] del primo holding register
#define LAST_HOLD_REG_ADDR		9   // indirizzo in reg[] dell'ultimo holding register

/*************************   ModBus Functions Available   ***************************/

//#define MB_FUNCTION_1 		// readCoils()
//#define MB_FUNCTION_2 		// readDiscreteInputs()
#define MB_FUNCTION_3 			// readHoldingReg()
#define MB_FUNCTION_4 			// readInputReG()
//#define MB_FUNCTION_5			// writeSingleCoil()
#define MB_FUNCTION_6 			// writeSingleReg()
//#define MB_FUNCTION_8			// diagnostic()
//#define MB_FUNCTION_15  		// writeMultipleCoil()
//#define MB_FUNCTION_16 		// writeMultipleReg()
//#define MB_FUNCTION_23		// read_writeMultipleReg()

/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

#FUSES WDT                 		// Watch Dog Timer
#FUSES HS                    	// High speed Osc (> 4mhz)
#FUSES PUT                   	// Power Up Timer
#FUSES NOPROTECT             	// Code not protected from reading
#FUSES MCLR                  	// Master Clear pin enabled
#FUSES NOCPD                 	// No EE protection
#FUSES BROWNOUT              	// Reset when brownout detected
#FUSES NOLVP					// Low Voltage Programming Disabled

/********************************************************************************/

#define FOSC 14745600

#use delay(clock=FOSC,restart_wdt)
// #use rs232(parity = e,xmit=PIN_B2,rcv=PIN_B1,bits=8,enable=PIN_B3,errors)

/********************************************************************************/

#define bit int1

/********************************************************************************/


/****************************************************************************************/

#define BITF_NEW_FRAME			BIT0
#define BITF_LISTEN				BIT1
#define BITF_WRITE_COIL			BIT2
#define BITF_WRITE_REG			BIT3
#define BITF_RESTART			BIT4
#define BITF_PARITY_ERR			BIT5	

/***************************************************************************************/

#if DEFAULT_BAUD_RATE == BAUD_RATE_0
	#define DEFAULT_BD_CODE  0
	
#elif DEFAULT_BAUD_RATE == BAUD_RATE_1
	#define DEFAULT_BD_CODE  1

#elif DEFAULT_BAUD_RATE == BAUD_RATE_2
	#define DEFAULT_BD_CODE  2

#elif DEFAULT_BAUD_RATE == BAUD_RATE_3
	#define DEFAULT_BD_CODE  3

#elif DEFAULT_BAUD_RATE == BAUD_RATE_4
	#define DEFAULT_BD_CODE  4
	
#elif DEFAULT_BAUD_RATE == BAUD_RATE_5
	#define DEFAULT_BD_CODE  5
#endif


/***************************************************************************************
Il coeff. 5/3 viene introdotto perche' t15 e' calcolato tra la fine della ricezione di un 
carattere e la fine della ricezione del successivo e non tra la fine di uno e l'inizio dell'altro.
Bisogna cioe' tenere in considerazione un intevallo di tempo pari ad un carattere in piu'.
/***************************************************************************************/

#define T_15_DEFAULT  			65535 - (5.0/3.0)*(11.0*3.0/2.0)*(FOSC/4.0/DEFAULT_BAUD_RATE)  
#define T_15_0					65535 - (5.0/3.0)*(11.0*3.0/2.0)*(FOSC/4.0/BAUD_RATE_0)	 	   		
#define T_15_1					65535 - (5.0/3.0)*(11.0*3.0/2.0)*(FOSC/4.0/BAUD_RATE_1)	 	   			 	
#define T_15_HIGH				65535 - (5.0/3.0)*750.0*(FOSC/4.0/10000000.0)	

/***************************************************************************************
Il coeff. 9/7 viene introdotto perche' t35 e' calcolato tra la fine della ricezione di un 
frame e la fine della ricezione del primo carattere del successivo frame e non tra la fine 
di un frame e l'inizio dell'altro.Bisogna cioe' tenere in considerazione un intevallo di tempo 
pari ad un carattere in piu'.
/***************************************************************************************/			    
																				  
#define T_35_DEFAULT  			65535 - (9.0/7.0)*(11.0*7.0/2.0)*(FOSC/4.0/DEFAULT_BAUD_RATE)
#define T_35_0					65535 - (9.0/7.0)*(11.0*7.0/2.0)*(FOSC/4.0/BAUD_RATE_0)		
#define T_35_1					65535 - (9.0/7.0)*(11.0*7.0/2.0)*(FOSC/4.0/BAUD_RATE_1)			
#define T_35_HIGH				65535 - (9.0/7.0)*1750.0*(FOSC/4.0/1000000.0)

/***************************************************************************************
T_TX e' il tempo che deve intercorrere tra la fine della ricezione di una richiesta e 
l'invio della relativa risposta.
/***************************************************************************************/			    
																				  
#define T_TX_DEFAULT  	65535 - 11.0*(FOSC/4.0/DEFAULT_BAUD_RATE)
#define T_TX_0					65535 - 11.0*(FOSC/4.0/BAUD_RATE_0)		
#define T_TX_1					65535 - 11.0*(FOSC/4.0/BAUD_RATE_1)			
#define T_TX_HIGH				65535 - 1750.0*(FOSC/4.0/1000000.0)

/***************************************************************************************/
#define DO0 					PIN_A0
#define DO1						PIN_A1
#define DO2 					PIN_A2
#define DO3 					PIN_A3

#define COL 					PIN_A4

#define DI0_2 					PIN_B6
#define DI1_3	 				PIN_B7

#define ADDRESS_PIN_1	    	PIN_B5
#define ADDRESS_PIN_2		   	PIN_B4	
#define ADDRESS_PIN_3	  		PIN_B0	

/***************************************************************************************/

#bit 	TMR1IF 	= 0x0C.0

/***************************************************************************************/


