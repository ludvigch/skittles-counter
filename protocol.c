///*==========================================================*/
/*  File: protocol.c
/*  Authors: Martin Falke & Ludvig Christensen
/*  Written: 2017
/*  Most recent update: 2017-02-26 by MF
/*
/*  The file protocol.c is used to contain all the functions,
/*  data and so on that are related to the protocol that is
/*  used to communicate with the I2C light sensor.
///*==========================================================*/

#include <pic32mx.h>
#include "protocol.h"
#include "mipslab.h"
#include "proj_defs.h"

// i2c methods

void i2c_init(){ // starts the i2c module and fixes the correct settings
    I2C1CON = 0x0; // reset the i2c configuration register just in case
    I2C1BRG = 0x0c2; // set the i2c baud rate to ~200kHz by dividing 40MHz w/ 0xc2
    I2C1STAT = 0x0; // reset the i2c status register just in case
    I2C1CONSET = PIC32_I2CCON_SIDL; // set the module to stop in idle mode
    I2C1CONSET = PIC32_I2CCON_ON; // set enable bit to 1 to start the module
}

void i2c_start(){
    i2c_wait();
    I2C1CONSET = PIC32_I2CCON_SEN; // set SEN to 1 to begin start condition with SCL and SDA
                                   // will be automatically reset by hardware (to 0)
    i2c_wait();
}

void i2c_stop(){
    i2c_wait();
    I2C1CONSET = PIC32_I2CCON_PEN; // set PEN to 1 to begin stop condition with SCL and SDA
                                   // will be automatically reset by hardware (to 0)
    i2c_wait();
}

void i2c_restart(){
    i2c_wait();
    I2C1CONSET = PIC32_I2CCON_RSEN; // set the RSEN bit to 1 to restart connection with SCL and SDA
                                    // will be automatically reset by hardware (to 0)
    i2c_wait();
}

void i2c_ack(){ //note: so far not used at all
    i2c_wait();
    I2C1CONCLR = PIC32_I2CCON_ACKDT; // set ACKDT to 0 for ACK
    I2C1CONSET = PIC32_I2CCON_ACKEN; // set ACKEN to start ack sequence (automatically performed and reset to 0 by hardware)
                                     // transmits the value of the ACKDT bit (cleared above)
}

void i2c_nak(){
    i2c_wait();
    I2C1CONSET = PIC32_I2CCON_ACKDT; // set ACKDT to 1 for NAK
    I2C1CONSET = PIC32_I2CCON_ACKEN; // set ACKEN to start ack sequence (automatically performed and reset to 0 by hardware)
                                     // transmits the value of the ACKDT bit (set above)
}
/* inspired by code written by Axel Isaksson */
/* https://github.com/is1200-example-projects/hello-temperature */
void i2c_wait(){ // wait until the bus is idle
  while( (I2C1CON & 0x1f) || I2C1STAT & PIC32_I2CSTAT_TRSTAT);
  // wait until the first 5bits in I2C1CON are reset by hardware.
  // this makes sure the hardware isnt sending any start conds for example.
  // also make sure the transmit status bit is 0.

}

unsigned char i2c_send(unsigned char data){
   i2c_wait();
   I2C1TRN = data; // add the data to the transmit buffer (32 bit register, uses only indices 0-7)
   i2c_wait(); // wait for the data to be sent
   return !(I2C1STAT & PIC32_I2CSTAT_ACKSTAT); // return the inverse of ackstat
                                               //returns 0x8000 if ack was received ~ "true"
                                               //returns 0x0000 if ack was not received ~ "false"
}

unsigned char i2c_recv(void){
   i2c_wait();
   I2C1CONSET = PIC32_I2CCON_RCEN; // turn on reception
   i2c_wait(); // wait for data to be recieved
   I2C1STATCLR = PIC32_I2CSTAT_I2COV; // Reset rcv overflow bit
   return I2C1RCV; //receive data register (32 bit register, uses only indices 0-7)
}

void i2c_test(){ // just a temporary test function
    int temp;
    i2c_init();
    do{ // this method of starting i2c is inspired from code by Axel Isaksson
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | 0x00); //0x00 is the sensor's enable register
    i2c_send(0x03); //bits 0 and 1, Power On and Enable the Analog-To-Digital converter (of light to RGBC color)
    i2c_stop();

    do{
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | 0x01); //0x01 is the sensor's timing register
    i2c_send(0xC0); //sets the integral cycles count to 64 (basically works like a pre-scaler)
                    //results in sensor's response time being 154ms
    i2c_stop();

    while(1){
      do{
          i2c_start();
      } while(!i2c_send(SENSOR_ADDR << 1));
      i2c_send(COMMAND_BIT | 0x16); //0x16 is the sensor's register for the low byte of red
      i2c_stop();

      do{
      i2c_start();
      } while(!i2c_send((SENSOR_ADDR << 1) | 1)); //sends sensor address and LSB as "read bit" = 1

      temp = i2c_recv();
      i2c_nak();
      i2c_stop();
      display_debug(&temp);
      display_update();
    }

}
