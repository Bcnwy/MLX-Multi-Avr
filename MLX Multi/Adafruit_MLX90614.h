/*************************************************** 

 ****************************************************/
#pragma once
#include "I2C.h"

#define MLX90614_I2CADDR 0x5A
// RAM
#define MLX90614_RAWIR1 0x04
#define MLX90614_RAWIR2 0x05
#define MLX90614_TA 0x06
#define MLX90614_TOBJ1 0x07
#define MLX90614_TOBJ2 0x08
// EEPROM
#define MLX90614_TOMAX 0x20
#define MLX90614_TOMIN 0x21
#define MLX90614_PWMCTRL 0x22
#define MLX90614_TARANGE 0x23
#define MLX90614_EMISS 0x24
#define MLX90614_CONFIG 0x25
#define MLX90614_ADDR 0x0E
#define MLX90614_ID1 0x3C
#define MLX90614_ID2 0x3D
#define MLX90614_ID3 0x3E
#define MLX90614_ID4 0x3F

class Adafruit_MLX90614  {
 public:
	Adafruit_MLX90614(uint8_t addr = MLX90614_I2CADDR);
	double readObjectTempC(void);
	double readAmbientTempC(void);
	uint8_t setEmissivity(float emis);
	float readEmissivity(void);
  
 private:
	I2c Wire; uint8_t _addr;
	float readTemp(uint8_t reg);
	uint8_t read16(uint8_t a , uint16_t * d);
	uint8_t writeEEPROM(uint8_t reg, int16_t data);
	uint8_t write16(uint8_t reg, int16_t data);
	uint8_t crc8 (uint8_t inCrc, uint8_t inData);
};

