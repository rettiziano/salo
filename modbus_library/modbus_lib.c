#define MODBUS_LIB_VER $Revision: 1.5 $
#define MODBUS_LIB_DATE $Date: 2006/11/10 13:09:11 $

/*************** #defines in use defined in <project_name>.h *******************


#define REG_SIZE	
#define BUFFER_SIZE
	
#define NUM_DIG_INPUTS
#define NUM_HOLD_REG
#define NUM_INPUT_REG
#define NUM_COUNTERS
#define NUM_COIL
#define MIN_REG_VAL
#define MAX_REG_VAL

#define DIG_INPUTS_ADDR		
#define COIL_ADDR           	
#define INPUT_REG_ADDR			
#define FIRST_COUNTER_ADDR 	 	
#define MESS_COUNT_ADDR     	
#define ERR_COUNT_ADDR      	
#define EXC_COUNT_ADDR      	
#define SLAVE_COUNT_ADDR 
#define SLAVE_NO_RESP_ADDR    	   		
#define FIRST_HOLD_REG_ADDR		
#define LAST_HOLD_REG_ADDR		


/*******************************************************************************/
#define BIT0    1
#define BIT1    2
#define BIT2    4
#define BIT3    8
#define BIT4    16
#define BIT5    32
#define BIT6    64
#define BIT7    128
#define BIT8    256
#define BIT9    512
#define BIT10   1024
#define BIT11   2048
#define BIT12   4096
#define BIT13   8192
#define BIT14   16384
#define BIT15   32768

unsigned int16 reg[REG_SIZE];
unsigned char buffer[BUFFER_SIZE];

unsigned char i;
unsigned char flagsm;
unsigned int16 count_35;
unsigned int16 count_tx;

#define BFM_NEW_FRAME          	BIT0
#define BFM_LISTEN             	BIT1
#define BFM_RISPOSTA_INVIATA		BIT2
#define BFM_RESTART           	BIT3
#define BFM_WRITE_REG						BIT4
#define	BFM_WRITE_COIL         	BIT5
#define BFM_SYNC_FOUND					BIT6
#define BFM_PARITY_ERR					BIT7

extern unsigned int16 coda_uscite;

/*******************************************************************************/

bit CRC16
	(
	unsigned int16 dataLength,
	char check
	)   
{////////////////////////////////////////// CRC 16 ///////////////////////////////////////////
unsigned int16 CheckSum;												// check = 0 -> scrive CRC in Buffer
unsigned int16 j;
unsigned char lowCRC;
unsigned char highCRC;

CheckSum = 0xffff;                              // carga el registro en unos
for (j=0; j<dataLength; j++)                    // controlo
	{
	CheckSum = CheckSum^(unsigned int16)buffer[j];// hace xor con carac. que entra con check y carga en check
	for(i=8;i>0;i--)                            // decrementa i,continua y repite este paso 8 veces
        if((CheckSum)&0x0001)
        	CheckSum = (CheckSum>>1)^0xa001;    // desplaza un bit a la derecha y xor
        else
        	CheckSum>>=1;
	}
highCRC = CheckSum>>8;
CheckSum<<=8;
lowCRC = CheckSum>>8;
if (check==1)
	{
	if ( (buffer[dataLength+1] == highCRC) & (buffer[dataLength] == lowCRC ))
  		return 1;  // OK
	else
  		return 0;  // Bad CRC
	}
else
	{
	buffer[dataLength] = lowCRC;     // Adjunta lowcrc al dato
	buffer[dataLength+1] = highCRC;  // Adjunta highcrc al dato
	return 1;
	}
}////////////////////////////////////////// CRC 16 ///////////////////////////////////////////


void clearCounters(void)			// Resetta tutti i contatori. I contatori devono stare in reg[] ed essere adiacenti uno all'altro
{
#if NUM_COUNTERS != 0 
char i;
for(i=0;i<NUM_COUNTERS;i++)
	reg[FIRST_COUNTER_ADDR + i] = 0;
#endif
}

/*******************************************************************************/

void writeOut(unsigned char dataL,unsigned char c)
{
if(buffer[0])		// responce only in unicast
	{
	CRC16(dataL,c);
		while(!TMR1IF);	// controlla se e' passato almeno t35 dall'ultimo char transitato nel bus
	for (i=0; i<(dataL+2); i++)
		{
		putc(buffer[i]);				
		}
	TMR1IF = 0;
	set_timer1(count_35);	// dopo che l'ultimo carattere e' stato inviato reimposta t35.
	}
restart_wdt();	
flagsm |= BFM_RISPOSTA_INVIATA;
}

/*******************************************************************************/

void writeBadRequest(unsigned char ex_code) //response for error in modbus poll
{
buffer[1] += 0x80;    		// error code
buffer[2] = ex_code;
reg[EXC_COUNT_ADDR] += 1;	// incrementa il relativo contatore
writeOut(3,0);
}
/******************************************************************************/

void readCoils(void)  // response for modbus 01 function (read coils) -> verified
{
unsigned char offset_buf,offset_reg;
unsigned int16 addr;
unsigned int16 n_coils;

if(!buffer[0])
	reg[EXC_COUNT_ADDR] += 1;
else
	{
	addr = buffer[2];
	addr <<= 8;
	addr |= buffer[3];               // addr = input starting address
	
	n_coils = buffer[4];
	n_coils <<= 8;
	n_coils |= buffer[5];           // n_input = n° of inputs
	
	if ((n_coils == 0) || (n_coils > NUM_COIL))
		{
		writeBadRequest(3);
	  	return;
		}
	
	if (addr + n_coils > NUM_COIL)
		{
		writeBadRequest(2);
	  	return;
		}
		
	buffer[2] = n_coils / 8;		// input status bytes count	
	if(n_coils%8)
		buffer[2] += 1;	
	  
	for(i=0;i<buffer[2];i++)		// input status bytes reset
		buffer[i+3] = 0;
	
	for (i=0;i<n_coils;i++)
		{
		offset_reg = (i+addr)/16;
		offset_buf = i/8;
		if (reg[COIL_ADDR + offset_reg] & BIT0 << ((addr+i)%16))   //  n_inputs position = bit position in byte value	
			buffer[3 + offset_buf] |=  (BIT0 << (i%8));     //  1
		else
			buffer[3 + offset_buf] &= ~(BIT0 << (i%8));    	//  0
		}
	writeOut((buffer[2]+3),0);
	}
}
/******************************************************************************/

void readDiscreteInputs(void)  // response for modbus 02 function (read discrete inputs) -> verified
{
unsigned char offset_reg,offset_buf;
unsigned int16 addr;
unsigned int16 n_input;

if(!buffer[0])
	reg[EXC_COUNT_ADDR] += 1;
else
	{
	addr = buffer[2];
	addr <<= 8;
	addr |= buffer[3];               // addr = input starting address
	
	n_input = buffer[4];
	n_input <<= 8;
	n_input |= buffer[5];           // n_input = n° of inputs
	
	if ((n_input == 0) || (n_input > NUM_DIG_INPUTS))
		{
		writeBadRequest(3);
	  	return;
		}
	
	if (addr + n_input > NUM_DIG_INPUTS)
		{
		writeBadRequest(2);
	  	return;
		}
		
	buffer[2] = n_input / 8;		// input status bytes count	
	if(n_input%8)
		buffer[2] += 1;	
	  
	for(i = 0;i<buffer[2];i++)		// input status bytes reset
		buffer[i+3] = 0;
	
	for (i=0;i<n_input;i++)
		{
		offset_reg = (i+addr)/16;	
		offset_buf = i/8;
		if (reg[DIG_INPUTS_ADDR + offset_reg] & BIT0 << ((addr+i)%16))                         //  n_inputs position = bit position in byte value
			buffer[3 + offset_buf] |=  (BIT0 << (i%8));    //  1
		else
			buffer[3 + offset_buf] &= ~(BIT0 << (i%8));    //  0
		}
	writeOut((buffer[2]+3),0);
	}
}
/*******************************************************************************/

void readHoldingReg(void) //response for modbus 03 function (read holding registers) -> verified
{//////////////////////////// read holding registers ////////////////////////////////////////////
unsigned int16 addr;
unsigned int16 n_reg;

if(!buffer[0])
	reg[EXC_COUNT_ADDR] += 1;
else
	{///////////////////////////////////   non siamo in broadcast
	addr = buffer[2];
	addr <<= 8;
	addr |= buffer[3];        // addr = register starting address
	
	n_reg = buffer[4];
	n_reg <<= 8;
	n_reg |= buffer[5];       // n_reg = n° of registers
	
	if (n_reg > NUM_HOLD_REG)
		{////  il numero dei registri richiesti supera quello dei registri disponibili
		writeBadRequest(3);
	   	return;
	 	}////  il numero dei registri richiesti supera quello dei registri disponibili

	if ((addr > LAST_HOLD_REG_ADDR) || (addr + n_reg)> NUM_HOLD_REG)
		{/// l'indirizzo del registro richiesto scavalca l'array dei registri disponibili
		writeBadRequest(2);
	   	return;
		}/// l'indirizzo del registro richiesto scavalca l'array dei registri disponibili
	
	buffer[2] = 2*n_reg; // byte count -> 1 reg = 2 bytes
	
	for (i=0;i<n_reg;i++)
		{/// prepara nel buffer i dati da scrivere
		buffer[i*2+3]   = reg[FIRST_HOLD_REG_ADDR + addr + i] >> 8; // read High register
		buffer[i*2+3+1] = reg[FIRST_HOLD_REG_ADDR + addr + i];    	// read Low register
		}/// prepara nel buffer i dati da scrivere	
	reg[DI_ADDR] &=~ 0x000f;
	writeOut((buffer[2]+3),0);
	}///////////////////////////////////   non siamo in broadcast
}//////////////////////////// read holding registers ////////////////////////////////////////////



void readInputReg(void) //response for modbus 04 function (read input registers) -> verified
{
unsigned int16 addr;
unsigned int16 n_reg;

if(!buffer[0])
	reg[EXC_COUNT_ADDR] += 1;
else
	{
	addr = buffer[2];
	addr <<= 8;
	addr |= buffer[3];        // addr = register starting address
	
	n_reg = buffer[4];
	n_reg <<= 8;
	n_reg |= buffer[5];       // n_reg = n° of registers
	
	if (n_reg > NUM_INPUT_REG)
		{
		writeBadRequest(3);
	   	return;
	 	}
	#if NUM_INPUT_REG != 0
	if (addr > NUM_INPUT_REG - 1)
		{
		writeBadRequest(2);
	   	return;
		}
	#endif
		
	buffer[2] = 2*n_reg; // byte count -> 1 input reg = 2 bytes 
	
	for (i=0;i<n_reg;i++)
		{
		buffer[i*2+3]   = reg[INPUT_REG_ADDR+addr+i] >> 8;  // write  High register
		buffer[i*2+3+1] = reg[INPUT_REG_ADDR+addr+i];    	// write  Low registers
		}
	writeOut((buffer[2]+3),0);
	}
}
/***************************************************************************************/

void writeSingleCoil(void) // Response for modbus 05 function write single coil -> verified
{
unsigned char offset;
unsigned int16 addr;
unsigned int16 val;

addr = buffer[2];   
addr <<= 8;
addr |= buffer[3];  // addr = coil position [0 ... 15]

val = buffer[4];
val <<= 8;
val |= buffer[5];   // val = value to write

offset = addr / 16;

if (val != 0x0000 && val!= 0xFF00)
	{
	writeBadRequest(3);
	return;
	}

#if (NUM_COIL > 0)
	if (addr >= NUM_COIL)
		{
		writeBadRequest(2);
		return;
		}		
#endif
	
if (val == 0xFF00)
	reg[COIL_ADDR + offset] |=  (BIT0 << (addr%16));    // write 1 
if (val == 0x0000)
	reg[COIL_ADDR + offset] &= ~(BIT0 << (addr%16));    // write 0
	
writeOut(6,0);
}
/*******************************************************************************/

void writeSingleReg(void)   // Response for modbus 06 function write single holding register -> verified
{
unsigned int16 addr;
unsigned int16 val;

addr = buffer[2];
addr <<= 8;
addr |= buffer[3];          // addr = register starting address

val = buffer[4];
val <<= 8;
val |= buffer[5];			// val = value to write

#if(MIN_REG_VAL > 0)
if (val < MIN_REG_VAL)
	{
	writeBadRequest(3);
	return;
	}
#endif

#if(MAX_REG_VAL < 65535)	
if (val > MAX_REG_VAL)
	{
	writeBadRequest(3);
	return;
	}
#endif
	
if (addr > LAST_HOLD_REG_ADDR)
	{
	writeBadRequest(2);
	return;
	}

if (coda_uscite == 0)
	{
	reg[FIRST_HOLD_REG_ADDR + addr] =  buffer[4];	 	    // write High register
	reg[FIRST_HOLD_REG_ADDR + addr] <<= 8;
	reg[FIRST_HOLD_REG_ADDR + addr] |=  buffer[5];		 	// write Low register
	}

writeOut(6,0);
}
/*******************************************************************************/

void diagnostic(void) // Response for modbus 08 function diagnostic -> verified
{
unsigned char sub_f;
unsigned int16 val;

sub_f = buffer[2];
sub_f <<= 8;
sub_f |= buffer[3];   					// sub_f = subfunction code
if (!(flagsm & BFM_LISTEN))     		// Normal Mode
	{
	switch(sub_f)
		{
		case 0:   						// Return Query Data
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					writeOut(6,0);
				break;
		case 1:							// Restart Comunication Option
				val = buffer[4];
				val <<= 8;
				val |= buffer[5];
				if (val == 0xFF00)
					reg[MESS_COUNT_ADDR] = 0;   	// Clear Comunication Event Log -> only Bus Message Count implemented
				else if(val != 0x00)
						{
						writeBadRequest(3);
						break;
						}
				writeOut(6,0);			// Responce before restart
				flagsm |= BFM_RESTART;
				break;
	  /*case 2:						// Return Diagnostic Register
	 			if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[DIAGN_REG_ADDR] >> 8;
					buffer[5] = reg[DIAGN_REG_ADDR];
					writeOut(6,0);
					}
				break;*/
		case 4:							// Force Listen Only Mode
				flagsm |= BFM_LISTEN;
            			break;
		case 10:						// Clear Diagnostic Register & Counters
				//reg[DIAGN_REG_ADDR] = 0;
				clearCounters();
				writeOut(6,0);
				break;
		case 11:						// Return Bus Message Count
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[MESS_COUNT_ADDR] >> 8;      // write High register
					buffer[5] = reg[MESS_COUNT_ADDR];    		// write Low register
					writeOut(6,0);
					}
				break;
		case 12:						// Return Bus Error (CRC) Comunication Count
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[ERR_COUNT_ADDR] >> 8;       // write High register
					buffer[5] = reg[ERR_COUNT_ADDR];     		// write Low register
					writeOut(6,0);
					}
				break;
		case 13:						// Return Bus Exception Error Count
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[EXC_COUNT_ADDR] >> 8;   	// write High register
					buffer[5] = reg[EXC_COUNT_ADDR]; 			// write Low register
					writeOut(6,0);
					}
				break;
		case 14:						// Return Slave Message Count
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[SLAVE_COUNT_ADDR] >> 8;   	// write High register
					buffer[5] = reg[SLAVE_COUNT_ADDR]; 			// write Low register
					writeOut(6,0);
					}
				break;
		case 15:						// Return Slave No-Responce Message Count
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[SLAVE_NO_RESP_ADDR] >> 8;   	// write High register
					buffer[5] = reg[SLAVE_NO_RESP_ADDR]; 			// write Low register
					writeOut(6,0);
					}
				break;
		case 18:						// Return Bus Character Overrun Count
				if(!buffer[0])
					reg[EXC_COUNT_ADDR] += 1;
				else
					{
					buffer[4] = reg[OVERRUN_COUNT_ADDR] >> 8;   	// write High register
					buffer[5] = reg[OVERRUN_COUNT_ADDR]; 			// write Low register
					writeOut(6,0);
					}
				break;
		default:
				writeBadRequest(1);
				break;
		}
	}
else                             // Listen Mode Activated
	if (sub_f == 1)
		{
		flagsm |= BFM_RESTART;
		flagsm ^= BFM_LISTEN;      // Return in normal mode
		}
}
/*********************************************************************************/

void writeMultipleCoil(void) // Response for modbus 15 function write multiple coils -> verified
{
unsigned char n_byte,offset_reg,offset_buf;
unsigned int16 addr;
unsigned int16 n_coil;

addr = buffer[2];   
addr <<= 8;
addr |= buffer[3];    	// addr first coil [0...15]

n_coil = buffer[4];
n_coil <<= 8;
n_coil |= buffer[5];  	// n_coil = n° outputs to write

n_byte = n_coil/8;  	// n° byte 
if(n_coil%8)
	n_byte += 1;

if (n_coil == 0 || n_coil > NUM_COIL || buffer[6] != n_byte)
	{
	writeBadRequest(3);
	return;
	}

#if (NUM_COIL > 0)	
if (addr>= NUM_COIL)
	{
	writeBadRequest(2);
	return;
	}
#endif
	
if ((addr+n_coil)>NUM_COIL)
	{
	writeBadRequest(2);
	return;
	}
	
for(i=0;i<n_coil;i++)
	{
	offset_reg = (addr+i)/16;
	offset_buf = i/8;
	if (buffer[7+offset_buf] & (BIT0 << (i%8)))     // coil position = bit position in byte value
		reg[COIL_ADDR + offset_reg] |=  (BIT0 << (addr+i)%16); 		// write 1 
	else    
		reg[COIL_ADDR + offset_reg] &= ~(BIT0 << (addr+i)%16);    	// write 0	
	}
	
writeOut(6,0);
}
/*********************************************************************************/ 

void writeMultipleReg(void) // Response for modbus 16 function preset multiple registers -> verified
{
unsigned int16 addr;
unsigned int16 n_reg;

addr = buffer[2];
addr = addr << 8;
addr |= buffer[3];  		// addr = register starting address

n_reg = buffer[4];
n_reg <<= 8;
n_reg |= buffer[5];
	if (n_reg <= 0 || n_reg > NUM_HOLD_REG || buffer[6] != n_reg*2)	
	{
	writeBadRequest(3);
	return;
	}
if (addr > LAST_HOLD_REG_ADDR || (addr + n_reg)> NUM_HOLD_REG)
	{
	writeBadRequest(2);
	return;
	} 

for (i=0; i<n_reg;i++)	     
	{
	reg[FIRST_HOLD_REG_ADDR + addr + i] = buffer[7+i*2];   // write High register
	reg[FIRST_HOLD_REG_ADDR + addr + i] <<= 8;
	reg[FIRST_HOLD_REG_ADDR + addr + i] |= buffer[7+1+i*2]; // write Low register
	}
writeOut(6,0);
}     

/******************************************************************************/

void read_writeMultipleReg(void) // Response for modbus 23 function read and write multiple registers -> verified
{
unsigned int16 r_addr,w_addr;
unsigned int16 wn_reg,rn_reg;

if(!buffer[0])
	reg[EXC_COUNT_ADDR] += 1;
else
	{
	r_addr = buffer[2];
	r_addr <<= 8;
	r_addr |= buffer[3];  		// addr = read starting address
	
	rn_reg = buffer[4];
	rn_reg <<= 8;
	rn_reg |= buffer[5];
	
	w_addr = buffer[6];
	w_addr <<= 8;
	w_addr |= buffer[7];  		// addr = write starting address
	
	wn_reg = buffer[8];
	wn_reg <<= 8;
	wn_reg |= buffer[9];
	
	if (rn_reg <= 0 || rn_reg > NUM_HOLD_REG || wn_reg <= 0 || wn_reg > NUM_HOLD_REG || buffer[10] != wn_reg*2)	
		{
		writeBadRequest(3);
		return;
		}
	if (r_addr > LAST_HOLD_REG_ADDR || (r_addr + rn_reg)> NUM_HOLD_REG || w_addr > LAST_HOLD_REG_ADDR || (w_addr + wn_reg)> NUM_HOLD_REG)
		{
		writeBadRequest(2);
		return;
		} 
	
	for (i=0; i<wn_reg;i++)	     
		{
		reg[FIRST_HOLD_REG_ADDR + w_addr + i] = buffer[11+i*2];   	// write High register
		reg[FIRST_HOLD_REG_ADDR + w_addr + i] <<= 8;
		reg[FIRST_HOLD_REG_ADDR + w_addr + i] |= buffer[11+1+i*2]; 	// write Low register
		}
		
	buffer[2] = 2*rn_reg; // byte count -> 1 reg = 2 bytes
	
	for (i=0;i<rn_reg;i++)
		{
		buffer[i*2+3]   = reg[FIRST_HOLD_REG_ADDR + r_addr + i] >> 8; 	// read High register
		buffer[i*2+3+1] = reg[FIRST_HOLD_REG_ADDR + r_addr + i];    	// read Low register
		}
	writeOut((buffer[2]+3),0);
	}
}     

/******************************************************************************/
