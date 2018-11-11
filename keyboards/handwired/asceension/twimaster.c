/*************************************************************************
* Title:    I2C master library using hardware TWI interface
* Author:   Peter Fleury <pfleury@gmx.ch>  http://jump.to/fleury
* File:     $Id: twimaster.c,v 1.3 2005/07/02 11:14:21 Peter Exp $
* Software: AVR-GCC 3.4.3 / avr-libc 1.2.3
* Target:   any AVR device with hardware TWI
* Usage:    API compatible with I2C Software Library i2cmaster.h
**************************************************************************/
#include <inttypes.h>
#include <compat/twi.h>

#include <i2cmaster.h>

#include "dbg.h"

/* define CPU frequency in Hz here if not defined in Makefile */
#ifndef F_CPU
#define F_CPU 16000000UL
#endif

/* I2C clock in Hz */
#define SCL_CLOCK  400000L

/*************************************************************************
 Initialization of the I2C bus interface. Need to be called only once
*************************************************************************/
void i2c_init(void)
{
  /* initialize TWI clock
   * minimal values in Bit Rate Register (TWBR) and minimal Prescaler
   * bits in the TWI Status Register should give us maximal possible
   * I2C bus speed - about 444 kHz
   *
   * for more details, see 20.5.2 in ATmega16/32 secification
   */

  TWSR = 0;     /* no prescaler */
  TWBR = 10;    /* must be >= 10 for stable operation */

}/* i2c_init */


/*************************************************************************
  Issues a start condition and sends address and transfer direction.

  param device_addr: 7bits
        rw_flag: 0=write, 1=read
  return:  0 device accessible
           1 failed to access device
*************************************************************************/
uint8_t i2c_start(uint8_t device_addr, uint8_t rw_flag)
{
  uint8_t   twst;
  
  /* send START condition */
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);
 
  /* wait until transmission completed */
  while(!(TWCR & (1<<TWINT)));

  /* check value of TWI Status Register. Mask prescaler bits. */
  twst = TWSR & 0xF8;
  if((twst != TW_START) && (twst != TW_REP_START)) {
      DBG_HEX("i2c err: start condition: status=", twst);
      return 1;
  }

  /* send device address */
  TWDR = (device_addr << 1) | rw_flag;         // bit7..1: device address, bit0: rw_flag
  TWCR = (1<<TWINT) | (1<<TWEN);

  /* wail until transmission completed and ACK/NACK has been received */
  while(!(TWCR & (1<<TWINT)));

  /* check value of TWI Status Register. Mask prescaler bits. */
  twst = TWSR & 0xF8;
  if((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK)) {
      DBG_HEX("i2c err: device address: status=", twst);
      return 1;
  }

  return 0;
}/* i2c_start */

/*************************************************************************
 Terminates the data transfer and releases the I2C bus
*************************************************************************/
void i2c_stop(void)
{
  /* send stop condition */
  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

  /* wait until stop condition is executed and bus released */
  while(TWCR & (1<<TWSTO));

}/* i2c_stop */


/*************************************************************************
  Send one byte to I2C device

  Input:    byte to be transfered
  Return:   0 write successful
            1 write failed
*************************************************************************/
uint8_t i2c_write(uint8_t data)
{
    uint8_t   twst;

  /* send data to the previously addressed device */
  TWDR = data;
  TWCR = (1<<TWINT) | (1<<TWEN);

  /* wait until transmission completed */
  while(!(TWCR & (1<<TWINT)));

  /* check value of TWI Status Register. Mask prescaler bits */
  twst = TWSR & 0xF8;
  if( twst != TW_MT_DATA_ACK) {
      DBG_HEX("i2c err: write: status=", twst);
      return 1;
  }
  
  return 0;
}/* i2c_write */


/*************************************************************************
 Read one byte from the I2C device, request more data from device

  Input:    *data read from I2C device
  Return:   0 read successful
            1 read failed
*************************************************************************/
uint8_t i2c_read_ack(uint8_t *data)
{
  uint8_t   twst;

  TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
  while(!(TWCR & (1<<TWINT)));

  /* check value of TWI Status Register. Mask prescaler bits */
  twst = TWSR & 0xF8;
  if( twst != TW_MR_DATA_ACK) {
      DBG_HEX("i2c err: read_ack: status=", twst);
      return 1; // failed
  }

  *data = TWDR;
  return 0;
}/* i2c_read_ack */


/*************************************************************************
 Read one byte from the I2C device, read is followed by a stop condition

  Input:    *data read from I2C device
  Return:   0 read successful
            1 read failed
*************************************************************************/
uint8_t i2c_read_nak(uint8_t *data)
{
  TWCR = (1<<TWINT) | (1<<TWEN);
  while(!(TWCR & (1<<TWINT)));

  *data = TWDR;
  return 0;
}/* i2c_read_nak */
