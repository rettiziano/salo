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

#define PIN_STP_DATA 		PIN_B1
#define PIN_STP_CK 		PIN_B4
#define PIN_STP_LE		PIN_B2
#define PIN_READ		PIN_B0
#define PIN_SER_PAR		PIN_B3

/************************************************************************************/

// #byte PORTA = 0x05
// #byte PORTB = 0x06


// #bit
// #bit	DIRECT_ = 0xF82.5 //PORTC.5 rxtx
// #bit	PARLA_ASCOLTA = 0xF82.5 //PORTC.5 rxtx
// #bit	PARLA_ASCOLTA = 0xF82.5 //PORTC.5 rxtx
// #bit	PARLA_ASCOLTA = 0xF82.5 //PORTC.5 rxtx
/************************************************************************************/
#define DISPLAY_0 0b11111100  // visualizza sul diplay 0
#define DISPLAY_1 0b01100000  // visualizza sul diplay 1
#define DISPLAY_2 0b11011010  // visualizza sul diplay 2
#define DISPLAY_3 0b11110010  // visualizza sul diplay 3
#define DISPLAY_4 0b01100110  // visualizza sul diplay 4
#define DISPLAY_5 0b10110110  // visualizza sul diplay 5
#define DISPLAY_6 0b00111110  // visualizza sul diplay 6
#define DISPLAY_7 0b11100000  // visualizza sul diplay 7
#define DISPLAY_8 0b11111110  // visualizza sul diplay 8
#define DISPLAY_9 0b11100110  // visualizza sul diplay 9


#define DISPLAY_A 0b11101110  // visualizza sul diplay A
#define DISPLAY_B 0b00111110  // visualizza sul diplay B
#define DISPLAY_C 0b10011100  // visualizza sul diplay C
#define DISPLAY_D 0b01111010  // visualizza sul diplay D
#define DISPLAY_E 0b10011110  // visualizza sul diplay E
#define DISPLAY_F 0b10001110  // visualizza sul diplay F
#define DISPLAY_P 0b11001110  // visualizza sul diplay P



#define _A 0	// posizione in hex array di lettera
#define _B 1	// posizione in hex array di lettera
#define _C 2	// posizione in hex array di lettera
#define _D 3	// posizione in hex array di lettera
#define _E 4	// posizione in hex array di lettera
#define _F 5	// posizione in hex array di lettera
#define _P 6	// posizione in hex array di lettera




