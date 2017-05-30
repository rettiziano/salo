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
#define D_0 0b11111100  // visualizza sul diplay 0
#define D_1 0b01100000  // visualizza sul diplay 1
#define D_2 0b11011010  // visualizza sul diplay 2
#define D_3 0b11110010  // visualizza sul diplay 3
#define D_4 0b01100110  // visualizza sul diplay 4
#define D_5 0b10110110  // visualizza sul diplay 5
#define D_6 0b00111110  // visualizza sul diplay 6
#define D_7 0b11100000  // visualizza sul diplay 7
#define D_8 0b11111110  // visualizza sul diplay 8
#define D_9 0b11100110  // visualizza sul diplay 9

#define D_P 0b11001110  // visualizza sul diplay P
#define D_E 0b10011110  // visualizza sul diplay E






