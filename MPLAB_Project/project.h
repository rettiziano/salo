/************************************************************************************/
/************************************************************************************/
/*********************       Parametri di Configurazione       **********************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/
/************************************************************************************/

#FUSES NOWDT // WDT                 		// Watch Dog Timer
#FUSES HS                    	// High speed Osc (> 4mhz)
#FUSES PUT                   	// Power Up Timer
#FUSES NOPROTECT             	// Code not protected from reading
#FUSES MCLR                  	// Master Clear pin enabled
#FUSES NOCPD                 	// No EE protection
#FUSES BROWNOUT              	// Reset when brownout detected
#FUSES NOLVP					// Low Voltage Programming Disabled

/************************************************************************************/

#use delay(clock=10MHz,restart_wdt)
// #use rs232(baud=115200, parity = N,xmit=PIN_B2,rcv=PIN_B1,bits=8)

/************************************************************************************/

#define bit int1

/************************************************************************************/

/************************************************************************************/
// #define PIN_SPIF PIN_A4
/************************************************************************************/

#define PIN_STP_DATA 	PIN_B1
#define PIN_STP_CK 		PIN_B4
#define PIN_STP_LE		PIN_B2
#define PIN_READ		PIN_B0
#define PIN_SER_PAR		PIN_B3

/* uscite ***************************************************************************/
#define EV1_CHIUDE		BIT0
#define EV1_APRE		BIT1
#define EV2_CHIUDE		BIT2
#define EV2_APRE		BIT3
#define EV3_CHIUDE		BIT4
#define EV3_APRE		BIT5
#define EV4_CHIUDE		BIT6
#define EV4_APRE		BIT7
#define EV5_CHIUDE		BIT8
#define EV5_APRE		BIT9
#define EV6_CHIUDE		BIT10
#define EV6_APRE		BIT11

#define LUCE_GIALLA		BIT12
#define LUCE_VERDE		BIT13
#define LUCE_ROSSA		BIT14

/* ingressi *************************************************************************/

#define MARCIA_START	BIT0
#define ARRESTO_STOP    BIT1
#define SWITCH_RESET    BIT2

#define P1_APRE		    BIT3
#define P1_CHIUDE	    BIT4
#define P2_APRE		    BIT5
#define P2_CHIUDE	    BIT6
#define P3_APRE		    BIT7
#define P3_CHIUDE	    BIT8
#define P4_APRE		    BIT9
#define P4_CHIUDE	    BIT10
#define P5_APRE		    BIT11
#define P5_CHIUDE	    BIT12
#define P6_APRE		    BIT13
#define P6_CHIUDE	    BIT14

#define SENSORI		    BIT15


/************************************************************************************/

// #byte PORTA = 0x05
// #byte PORTB = 0x06


// #bit
// #bit	DIRECT_ = 0xF82.5 //PORTC.5 rxtx
// #bit	PARLA_ASCOLTA = 0xF82.5 //PORTC.5 rxtx
// #bit	PARLA_ASCOLTA = 0xF82.5 //PORTC.5 rxtx
// #bit	PARLA_ASCOLTA = 0xF82.5 //PORTC.5 rxtx
/************************************************************************************/
// questi sono invertiti, andrebbe ruotato tutto
#define DISPLAY_n0 0b11111100  // visualizza sul diplay 0
#define DISPLAY_n1 0b01100000  // visualizza sul diplay 1
#define DISPLAY_n2 0b11011010  // visualizza sul diplay 2
#define DISPLAY_n3 0b11110010  // visualizza sul diplay 3
#define DISPLAY_n4 0b01100110  // visualizza sul diplay 4
#define DISPLAY_n5 0b10110110  // visualizza sul diplay 5
#define DISPLAY_n6 0b00111110  // visualizza sul diplay 6
#define DISPLAY_n7 0b11100000  // visualizza sul diplay 7
#define DISPLAY_n8 0b11111110  // visualizza sul diplay 8
#define DISPLAY_n9 0b11100110  // visualizza sul diplay 9
#define DISPLAY_nA 0b11101110  // visualizza sul diplay A
#define DISPLAY_nB 0b00111110  // visualizza sul diplay B
#define DISPLAY_nC 0b10011100  // visualizza sul diplay C
#define DISPLAY_nD 0b01111010  // visualizza sul diplay D
#define DISPLAY_nE 0b10011110  // visualizza sul diplay E
#define DISPLAY_nF 0b10001110  // visualizza sul diplay F


#define DISPLAY_A 0b11101110  // visualizza sul diplay A
#define DISPLAY_B 0b00111110  // visualizza sul diplay B
#define DISPLAY_C 0b10011100  // visualizza sul diplay C
#define DISPLAY_D 0b01111010  // visualizza sul diplay D
#define DISPLAY_E 0b10011110  // visualizza sul diplay E
#define DISPLAY_F 0b10001110  // visualizza sul diplay F
#define DISPLAY_P 0b11001110  // visualizza sul diplay P
#define DISPLAY_I 0b00100000  // visualizza sul diplay I
#define DISPLAY_N 0b00101010  // visualizza sul diplay n
#define DISPLAY_T 0b00011110  // visualizza sul diplay t
#define DISPLAY_R 0b00001010  // visualizza sul diplay r
#define DISPLAY_S 0b10110110  // visualizza sul diplay S
#define DISPLAY_L 0b00011100  // visualizza sul diplay S
#define DISPLAY_OFF 0 

enum _pos_ { 	// posizione in hex array di lettera
	_A = 0,
	_B,	
	_C, 
	_D, 
	_E, 
	_F,
	_P, 
	_I, 
	_N, 
	_T,
	_R,
	_S,
	_L,
};	



enum _stati_ {
	ST_IDLE = 0,
	ST_ALARM_1,
	ST_START,
	ST_WAIT,
	ST_ARRESTO,
	
	
	
};	

enum _err_ {
	NO_ERROR = 0,
	ERROR_1,	// errore ...
	ERROR_2,	// errore ... 
	ERROR_3,	// errore ...
};