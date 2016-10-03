//
// software I2C master, including clock stretching. 
//

// SCL_IN = SDA_IN = 0;

// We use a small delay routine between SDA and SCL changes to give a clear sequence on the I2C bus. 
// This is nothing more than a subroutine call and return.
void i2c_dly(void)
{
}

// The following 4 functions provide the primitive start, stop, read and write sequences. 
// All I2C transactions can be built up from these.

void i2c_start(void)
{
  bSDA = 1;             // i2c start bit sequence
  i2c_dly();
  bSCL = 1;
  i2c_dly();
  bSDA = 0;
  i2c_dly();
  bSCL = 0;
  i2c_dly();
}

void i2c_stop(void)
{
  bSDA = 0;             // i2c stop bit sequence
  i2c_dly();
  bSCL = 1;
  i2c_dly();
  bSDA = 1;
  i2c_dly();
}


unsigned char i2c_read(char ack)
{
char x, d=0;
  bSDA = 1; 
  for(x=0; x<8; x++) {
    d <<= 1;
    do {
      bSCL = 1;
    }
    while(bSCL==0);    // wait for any SCL clock stretching
    i2c_dly();
    if(bSDA) d |= 1;
    bSCL = 0;
  } 
  if(ack) bSDA = 0;
  else bSDA = 1;
  bSCL = 1;
  i2c_dly();             // send (N)ACK bit
  bSCL = 0;
  bSDA = 1;
  return d;
}

bit i2c_write(unsigned char d)
{
	char x;
	static bit b;
	for(x=8; x; x--) 
	{
	    if(d&0x80) bSDA = 1;
	    else bSDA = 0;
	    bSCL = 1;
	    d <<= 1;
	    bSCL = 0;
	}
    bSDA = 1;
    bSCL = 1;
    i2c_dly();
    b = bSDA;          // possible ACK bit
    bSCL = 0;
    return b;
}

/*
// The 4 primitive functions above can easily be put together to form complete I2C transactions. 
// Here's and example to start an SRF08 ranging in cm:

i2c_start();              // send start sequence
i2c_tx(0xE0);             // SRF08 I2C address with R/W bit clear
i
2c_tx(0x00);             // SRF08 command register address
i2c_tx(0x51);             // command to start ranging in cm
i2c_stop();               // send stop sequence
*/


/*
// Now after waiting 65mS for the ranging to complete (I've left that to you) the following example 
// shows how to read the light sensor value from register 1 and the range result from registers 2 & 3.

i2c_start();              // send start sequence
i2c_tx(0xE0);             // SRF08 I2C address with R/W bit clear
i2c_tx(0x01);             // SRF08 light sensor register address
i2c_start();              // send a restart sequence
i2c_tx(0xE1);             // SRF08 I2C address with R/W bit set
lightsensor = i2c_rx(1);  // get light sensor and send acknowledge. Internal register address will increment automatically.
rangehigh = i2c_rx(1);    // get the high byte of the range and send acknowledge.
rangelow = i2c_rx(0);     // get low byte of the range - note we don't acknowledge the last byte.
i2c_stop();               // send stop sequence

*/
