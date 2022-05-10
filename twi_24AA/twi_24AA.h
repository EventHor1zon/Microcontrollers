/*  Small set of functions for using the 24AA set of EEPROM memory chips
 *  uses Atmel's TWI (essentially I2C) to read and write data
 *  [TODO] add function to clear all flash mem? Add function to encrypt flash mem?
 */

#include "stdint.h"

#define START 0x08
#define MTX_ADDR_ACK 0x18
#define MTX_DATA_ACK 0x28
#define MRC_ADDR_ACK 0x40
#define MRC_DATA_ACK 0x50
#define MRC_DATA_NACK 0x58
#define STATUS (TWSR & 0xF8)

#define TEST_DATA 0x41;
#define TEST_ADDR_H 0x00;
#define TEST_ADDR_L 0x00;

#define SLA_ADDR_W 0xA0
#define SLA_ADDR_R SLA_ADDR_W+1

void set_rate();

void start_write();

void stop_write();

int addr_write(uint8_t SLA_ADDR);

int data_write(char data);

int data_read(uint8_t read_len, char *buffer_ptr);

int TWI(uint8_t SLA_ADDR, uint8_t MEM_ADDR_L, uint8_t MEM_ADDR_H, char *ptr, bool READ, uint8_t _len);
