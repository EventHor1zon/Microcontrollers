/**
 *  Small library to control the 24AA series of i2c EEPROM ICs 
 *      using AVR's Two Wire Interface
 *      https://ww1.microchip.com/downloads/en/DeviceDoc/20005202A.
 * 
 *  Bit of practice to learn how I2C works and programming with registers
 * 
 *  Built and tested with ATMega328p
 * 
 **/


#include "twi_24AA.h"

void set_rate(){
  TWBR=0xFF;
  TWSR=(1<<TWPS1)|(1<<TWPS0);
}

void start_write(){
    // sets the Two-wire register to send a start-bit onto the bus
    TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT))){ ; }
}

void stop_write(){
    // sets the two-wire register to send a stop bit onto the bus
    TWCR=(1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

int addr_write(uint8_t SLA_ADDR){
    // writes the address onto the bus
    TWDR=SLA_ADDR;                              // load addr into reg
    TWCR=(1<<TWINT)|(1<<TWEN);                    // send
    while(!(TWCR & (1<<TWINT))){ ; }              // wait for sent
    if(!((STATUS == (MTX_ADDR_ACK)) || STATUS == (MRC_ADDR_ACK))){             // check status reg for ack
        return STATUS;
      }
    return 0;                                     // return ok!
}

int data_write(char data){
    // writes a byte of data onto the bus
    TWDR=data;                                     // load data into reg
    TWCR=(1<<TWINT)|(1<<TWEN);
    while(!(TWCR & (1<<TWINT))){ ; }
    if(STATUS != MTX_DATA_ACK){
        return STATUS;
      }
    return 0;
}

int data_read(uint8_t read_len, char *buffer_ptr){
    // hopefully works! Read a byte of data from the bus. If it's the last byte to get, make sure to send a NACK to end data feed
    for(int i=0;i<=(read_len-1);i++)
    {
      TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWEA);
      while(!(TWCR & (1<<TWINT))){ ; }
      *buffer_ptr=(char )TWDR;                                      // read data from reg
      buffer_ptr++;
      if(STATUS != MRC_DATA_ACK){            // check register
        return STATUS;
      }
    }
    TWCR=(1<<TWINT)|(1<<TWEN);                              //  recv data & send NACK
    while(!(TWCR & (1<<TWINT))){ ; }              // check int set
    buffer_ptr=(char )TWDR;                                      // read data from reg
    if(STATUS != MRC_DATA_NACK){           // check status
        return STATUS;
      }
    return 0;                                    // return data byte
}

int TWI(uint8_t SLA_ADDR, uint8_t MEM_ADDR_L, uint8_t MEM_ADDR_H, char *ptr, bool READ, uint8_t _len){
    // TWI function to run read/write operation.
    // SLA_ADDR is the 7-bit address for the 24AA chip. Make sure the last bit is 0 always, 1 will be added to the address for writing
    // for read or write instructions need a 2-part memory addresses MEM_ADDR_L & H. Read from or write to address.
    // for read or write need char* data_ptr (empty buffer for read, buffer containing bytes for write)
    // bool READ - 1 for read, 0 for write
    // for read, need a data length to read - for write, use strlen(data) or similar
    start_write();                                                            // all interactions start with a start bit
    if(addr_write(SLA_ADDR)) { return STATUS; }                     // "      "           "     "  target address (write mode)
    if(data_write(MEM_ADDR_H)) { return STATUS; }                   // "      "           "     "  high memory address on target
    if(data_write(MEM_ADDR_L)) { return STATUS; }                   //  "     "           "     "  low memory address on target
    if(READ){                                                                   // if read...
      start_write();                                                                      // send another start bit
      if(addr_write(SLA_ADDR_R)) { return STATUS; }               // send address w. read bit set ( now into master read mode)
      data_read(_len, ptr);
    } else {                                                                  // if write;
      for(int i=0;i<_len;i++){
        char c=*ptr;                                                              // for char in data to write
        if(data_write(c)) { return STATUS; }      // write the char at data_ptr
        *ptr++;                                                          // increment the data ptr
      }
    }
    stop_write();                                                             // send the stop bit
    return 0;                                                                 // return ok!
}
