/********************************************************************************************/

unsigned int16 count_15; 	// Max inter-character time
unsigned int16 count_35_isr;	// Min inter-frame time	

/********************************************************************************************/

void setDefaultUart(void)				// setta T15,T35 e il baud rate in base al DEFAULT_BAUD_RATE 
{										// impostato  nel file.h
/*
#if (DEFAULT_BAUD_RATE > 19200)
	{
	count_15 = T_15_HIGH;
	count_35 = T_35_HIGH;
	count_tx = T_TX_HIGH;
	count_35_isr = 65535 + count_35 - count_15;
	}	
#else
*/
	{
	count_15 = T_15_DEFAULT;
	count_35 = T_35_DEFAULT;
	count_tx = T_TX_DEFAULT;
	count_35_isr = 65535 + count_35 - count_15;
	}
//#endif
			
switch (DEFAULT_BAUD_RATE)
	{
	case 9600:
		set_uart_speed (9600);
		reg[UART_ADDR] = 0x00;					
		break;
	case 19200:
		set_uart_speed (19200);	
		reg[UART_ADDR] = 0x01;				
		break;
	case 28800:
		set_uart_speed (28800);	
		reg[UART_ADDR] = 0x02;				
		break;
	case 38400:
		set_uart_speed (38400);	
		reg[UART_ADDR] = 0x03;				
		break;
	case 57600:
		set_uart_speed (57600);	
		reg[UART_ADDR] = 0x04;				
		break;
	case 115200:
		set_uart_speed (115200);	
		reg[UART_ADDR] = 0x05;				
		break;
	default:
		set_uart_speed (DEFAULT_BAUD_RATE);		
		reg[UART_ADDR] = DEFAULT_BD_CODE;				
	}

set_timer1(count_35); 			
}
/********************************************************************************************/

void configUart(void)							// reimposta il baud rate,T15 e T35 quando 
{																	// l'holding register di configurazione della UART 
unsigned char br_n = 0;						// viene modificato

br_n  = reg[UART_ADDR] & BIT0;  	// Read Baud Rate configuration bits
br_n |= reg[UART_ADDR] & BIT1;  	// "
br_n |= reg[UART_ADDR] & BIT2;  	// "

switch (br_n)
	{
	case 0:
		set_uart_speed (9600);	
		break;
	case 1:
		set_uart_speed (19200);	
		break;
	case 2:
		set_uart_speed (28800);	
		break;
	case 3:
		set_uart_speed (38400);	
		break;
	case 4:
		set_uart_speed (57600);	
		break;
	case 5:
		set_uart_speed (115200);	
		break;
	default:
		set_uart_speed (DEFAULT_BAUD_RATE);		
	}	
setDefaultUart();

/*
if (br_n > 5)				// fuori range -> Default
	setDefaultUart();
else if (br_n == 0) 
	{
	count_15 = T_15_0;								
	count_35 = T_15_0;
	count_tx = T_TX_0;							// according to the baud rate, waiting times t35 e t15 are set
	count_35_isr = 65535 + count_35 - count_15;		
	}						
else if (br_n == 1) 
	{
	count_15 = T_15_1;
	count_35 = T_15_1;
	count_tx = T_TX_1;
	count_35_isr = 65535 + count_35 - count_15;
	}	
else
	{
	count_15 = T_15_HIGH;
	count_35 = T_35_HIGH;
	count_tx = T_TX_HIGH;
	count_35_isr = 65535 + count_35 - count_15;
	}
*/
}				  					     		     					   


void function_call()
{//////////////////////////////// function_call //////////////////////////////////////////
if (!(flagsm & BFM_LISTEN)) 
	{	// Normal Mode, NON lietsn mode
	switch (buffer[1])
		{// switch delle funzioni disponibili in modalita NON_LISTEN
	#ifdef MB_FUNCTION_1	
		case 1: readCoils();			
				break;
	#endif
	#ifdef MB_FUNCTION_2	
		case 2: readDiscreteInputs();	// Needed for communication init 
				break;	
	#endif
	#ifdef MB_FUNCTION_3			
		case 3: readHoldingReg();
				break;	
	#endif
	#ifdef MB_FUNCTION_4			
		case 4: readInputReg();
				break;
	#endif
	#ifdef MB_FUNCTION_5	
      	case 5: writeSingleCoil();
      			flagsm |= BFM_WRITE_COIL;
				break;	
	#endif
	#ifdef MB_FUNCTION_6		
		case 6: writeSingleReg();
			    flagsm |= BFM_WRITE_REG;
			    flagsm |= BFM_WRITE_COIL;
			    break;	
	#endif
	#ifdef MB_FUNCTION_8	    
		case 8: diagnostic();
				break;	
	#endif
	#ifdef MB_FUNCTION_15		
      	case 15:writeMultipleCoil();
				flagsm |= BFM_WRITE_COIL;
      			break;	
    #endif
    #ifdef MB_FUNCTION_16		
      	case 16:writeMultipleReg();
				flagsm |= BFM_WRITE_REG;
				flagsm |= BFM_WRITE_COIL;
				break;	
 	#endif
 	#ifdef MB_FUNCTION_23	
		case 23:read_writeMultipleReg();	
				flagsm |= BFM_WRITE_REG;
				break;
	#endif	
		default:writeBadRequest(1);
				break;
		}// switch delle funzioni disponibili in modalita NON_LISTEN
	}	// Normal Mode, NON lietsn mode
else
	{ // Listen Mode Activated
	if (buffer[1] == 8)
		diagnostic();
	} // Listen Mode Activated
}//////////////////////////////// function_call //////////////////////////////////////////



