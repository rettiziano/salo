unsigned int8 DISP(unsigned int8 output_data)
{
	unsigned int8 i;

//	unsign8ed int8 position;	// importante partire dall'ultima posizione

	#define FIRST_POSITION 0 
	#define LAST_POSITION 7+
	
	
			
	output_low(PIN_B2);		// abilito il display
	delay_us(10); 			// aspetta

	

	for(i=FIRST_POSITION; i<=LAST_POSITION;i++) 
	{// scrive 8 bit sullo shift register del display
		
		// output_bit (PIN, val); assegna val a PIN
		output_bit (PIN_STP_DATA, ((output_data & BIT7) != 0)); // scrive il valore dell'ultimo bit sul pin di uscita
		delay_us(10); // aspetta
		
		
		output_data <<= 1;	// RoL Circular rotation left
		delay_us(10); 		// aspetta
		
		output_high(PIN_A1);	// clock salita display
		delay_us(10); 		// aspetta
		output_low(PIN_A1);	// clock discesa display			
	}

	output_high(PIN_B2);	// disabilito il display
	

	return ( );
	
}