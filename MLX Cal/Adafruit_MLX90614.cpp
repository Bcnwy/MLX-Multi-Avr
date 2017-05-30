/*************************************************** 
  This is a library for the MLX90614 Temp Sensor

  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_MLX90614.h"
#include "uart.h"
#include "util/delay.h"

/*********************************************************************/
template<typename V>
inline V constrain(V x, V a, V b) {
	return((x < a)?a:(b<x)?b:x);
}
/*********************************************************************/ 
Adafruit_MLX90614::Adafruit_MLX90614(uint8_t i2caddr) {
  _addr = i2caddr;
}
/*********************************************************************/
double Adafruit_MLX90614::readObjectTempC(void) {
  return readTemp(MLX90614_TOBJ1);
}

double Adafruit_MLX90614::readAmbientTempC(void) {
  return readTemp(MLX90614_TA);
}

float Adafruit_MLX90614::readTemp(uint8_t reg) {
  float temp;
  uint16_t t;
  read16(reg, &t);
  temp = t * .02;
  temp -= 273.15;
  return temp;
}
/*********************************************************************/
uint8_t Adafruit_MLX90614::setEmissivity(float emis)
{
	// Make sure emissivity is between 0.1 and 1.0
	if ((emis > 1.0) || (emis < 0.1))
	return 0; // Return fail if not
	// Calculate the raw 16-bit value:
	uint16_t ke = uint16_t(65535.0 * emis);
	ke = constrain<uint16_t>(ke, 0x2000, 0xFFFF);

	// Write that value to the ke register
	return writeEEPROM(MLX90614_EMISS, (int16_t)ke);
}

float Adafruit_MLX90614::readEmissivity(void)
{
	uint16_t ke;
	if (read16(MLX90614_EMISS, &ke))
	{
		// If we successfully read from the ke register
		// calculate the emissivity between 0.1 and 1.0:
		return (((float)ke) / 65535.0);
	}
	return 1; // Else return fail
}

/*********************************************************************/

uint8_t Adafruit_MLX90614::read16(uint8_t a , uint16_t * d) {
  uint16_t ret;
  Wire.beginTransmission(_addr); // start transmission to device 
  Wire.write(a); // sends register address to read from
  Wire.endTransmission(false); // end transmission
  
  Wire.requestFrom(_addr, (uint8_t)3);// send data n-bytes read
  ret = Wire.read(); // receive DATA
  ret |= Wire.read() << 8; // receive DATA

  uint8_t pec = Wire.read();
  *d = ret;
  return 0;
}
uint8_t Adafruit_MLX90614::write16(uint8_t reg, int16_t data)
{
	uint8_t crc;
	uint8_t lsb = data & 0x00FF;
	uint8_t msb = (data >> 8);
	
	crc = crc8(0, (_addr << 1));
	crc = crc8(crc, reg);
	crc = crc8(crc, lsb);
	crc = crc8(crc, msb);
	
	Wire.beginTransmission(_addr);
	Wire.write(reg);
	Wire.write(lsb);
	Wire.write(msb);
	Wire.write(crc);
	return Wire.endTransmission(true);
}
uint8_t Adafruit_MLX90614::writeEEPROM(uint8_t reg, int16_t data)
{
	// Clear out EEPROM first:
	if (write16(reg, 0) != 0)
	return 1; // If the write failed, return 1
	_delay_ms(5); // Delay tErase
	
	uint8_t i2cRet = write16(reg, data);
	_delay_ms(5); // Delay tWrite
	
	if (i2cRet == 0)
	return 0;
	else
	return 1;
}

uint8_t Adafruit_MLX90614::crc8 (uint8_t inCrc, uint8_t inData)
{
	uint8_t i;
	uint8_t data;
	data = inCrc ^ inData;
	for ( i = 0; i < 8; i++ )
	{
		if (( data & 0x80 ) != 0 )
		{
			data <<= 1;
			data ^= 0x07;
		}
		else
		{
			data <<= 1;
		}
	}
	return data;
}

/*********************************************************************/
