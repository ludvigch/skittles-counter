///*==========================================================*/
/*  File: init.c
/*  Authors: Martin F & Ludvig C
/*  Written: 2017
/*  Most recent update: 2017-03-01 by MF
/*
/*  This file is intended for the initialization of registers,
/*  variables and the like.
///*==========================================================*/

#include <stdint.h>
#include <pic32mx.h>
#include "proj_defs.h"

void sensor_init();
void timer_init();
void switch_init();

void init(){

  //initializations
  display_init(); //init display on I/O shield

  //display_skittles();
  //int i = 0 TODO implement this with skittles fontmap
  //while(i++ < 1000){quicksleep(10000);}

  i2c_init(); // init i2c on the ChipKIT
  sensor_init(); // init the sensor
  timer_init(); //init the timer for interrupts (and its interrupt)
  switch_init(); //init the switch1 for interrupts (and its interrupt)
  enable_interrupt(); //enables interrupts
}

// name:    sensorInit()
// description: initiates and confgures the sensorInit
// args:    none
// return:  void
// PRECONDITIONS: I2C MUST be configured before calling this funciton
void sensor_init(){
    do{
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | 0x00);
    i2c_send(0x03); // set the enable and power on bits of the sensor
    i2c_stop();

    do{
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | 0x01);
    i2c_send(0xF6); // set integration time of sensor to 24ms
    i2c_stop();

    do{
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | 0x0F);
    i2c_send(0x01); // set the gain
    i2c_stop();
}

void timer_init(){
  T2CON = 0; //resets timer2
  T2CON = (0x6 << 4); //prescaler to 1:64 (110) - bits 4-6
  TMR2 = 0; //resets timer register to 0
  PR2 = 62500;      //sets the period register to the amount of clock cycles that passes between each interrupt
                    //80 000 000/64 = 1 250 000 (once per second)
                    //1 250 000/20 = 62500 (20 times per second)

  //interrupts for timer2
  IPCSET(2) = 0xF; //sets priority level to 3 and subpriority level to 3, 1F is 11111 in binary (bit indices 0-4)
  IFSCLR(0) = 0x100; //makes sure the interrupt flag is cleared (bit index 8)
  IECSET(0) = 0x100; //enables the interrupt flag bit for timer2 (bit index 8)
}

void switch_init(){
  //BITS main prio: 26-28 sub prio 24-25
  IPCSET(1) = 0x1F << 24; //sets prio level to 7 and subpriority level to 3
  IFSCLR(0) = 0x80; //makes sure the interrupt flag is cleared (bit index 7)
  IECSET(0) = 0x80; //enables the interrupt flag bit for switch1 (bit index 7)
}
