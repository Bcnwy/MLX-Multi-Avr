#pragma once

 #include <inttypes.h>  // AVR toolchain doesn't offer cinttypes header
 #include "config.h"


 #ifndef F_CPU
 #error "F_CPU not defined for I2C.h"
 #endif
 
 extern "C" {
     #include "i2cmaster.h"
	  }
 class I2c
 {
 public:
    I2c()
    {
	    i2c_init();
    }
     void beginTransmission(uint8_t address)
     {
         m_bufferIndex = 0;

         i2c_start((address << 1) | I2C_WRITE);
     }

     void write(uint8_t byte)
     {
         if (m_bufferIndex == c_bufferSize)
             return;

         m_buffer[m_bufferIndex++] = byte;
    }

    uint8_t endTransmission(bool stop = true)
    {
         for (uint8_t index = 0; index < m_bufferIndex; ++index) {
             if (i2c_write(m_buffer[index]) != 0)
                 return 1;
         }

        if (stop)i2c_stop();
			 
		return 0;	
     }

     uint8_t requestFrom(uint8_t address, uint8_t quantity, bool stop = true)
     {
        // quantity = Sensors::min(quantity, c_bufferSize);
         if (quantity == 0)
             return 0;

         if (i2c_start((address << 1) | I2C_READ) != 0)
             return 0;

        m_bufferIndex = 0;
         while (--quantity > 0) {
             m_buffer[m_bufferIndex++] = i2c_readAck();
         }
         m_buffer[m_bufferIndex++] = i2c_readNak();
         m_bufferReadIndex = 0;

         if (stop)
             i2c_stop();
         return m_bufferIndex;
     }

     uint8_t read()
     {
         if (m_bufferReadIndex == m_bufferIndex)
             return 0;
        return m_buffer[m_bufferReadIndex++];
     }

 private:
     //static I2c s_instance;

     static const uint8_t c_bufferSize = 8;
     uint8_t m_buffer[c_bufferSize];
     uint8_t m_bufferIndex = 0;
     uint8_t m_bufferReadIndex = 0;
 };
  // \addtogroup libtarget_i2c