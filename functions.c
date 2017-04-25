///*==========================================================*/
/*  File: functions.c
/*  Authors: Martin Falke & Ludvig Christensen
/*  Written: 2017
/*  Most recent update: 2017-03-01 by LC+MF
/*
/*  functions.c is used to implement the definitions of functions
/*  that will be used by update that are not direct implementation
/*  of the protocol used to communicate with the light sensor.
///*==========================================================*/

#include <pic32mx.h>
#include "proj_defs.h"
#include <stdlib.h>

//stores the values of the counters in a string
char counters[16];

//COUNTERS (colors)
volatile char red = 0;
volatile char green = 0;
volatile char yellow = 0;
volatile char purple = 0;
volatile char orange = 0;
volatile char total = 0;
#define OFFSET  (48) //decimal ASCII code for 0 is 48

//Used to determine which counter should be incremented when the same color has been detected for 1 sec
volatile char inColorId;
//Counter for interrupts
volatile unsigned char interrupts;

char wait_boolean = 0; //implements wait if counter recently incremented

/*******************************************/
/* some functions and their implementation */
/*******************************************/

// name: 	  timer_isr()
// description:	interrupt service routine for the timer that is used
//              to makes sure colors are being read correctly
void timer_isr(){

  if(IFS(0) & 0x100){ //when the timer interrupts

        IFSCLR(0) = 0x100; //clears the interrupt flag
        if(++interrupts == 20)
        {
          //1.0 sec has passed (each interrupt is 0.05 sec and we count 20 interrupts, 20*0.05 = 1.0)
          timer_reset();
          incrementCounter();
        }
    }

    if(IFS(0) & 0x80){ //when switch1 interrupts
      /* resets all the counter variables */
      red = 0; green = 0;
      yellow = 0; purple = 0;
      orange = 0; total = 0;
      IFSCLR(0) = 0x80; //clears interrupt flag

      timer_reset(); //resets and disables timer so it won't increment right after resetting
    }

}


// name: 	  timer_reset()
// description:	resets the timer register and disables it, also resets the counter for interrupts
void timer_reset(){
    T2CONCLR = (0x1<<15); //disables timer2 y tho????
    TMR2 = 0; //resets the counter
    interrupts = 0; //resets the interrupt counter
}

// name: 	  incrementCounter()
// description:	uses global variable to increment its corresponding color
void incrementCounter(){
  /* S - G - L - B - O */
  /* ASCII values (not needed since case works with 'c'/characters)
      S - 83
      G - 71
      L - 76
      B - 66
      O - 79

      If statements since there currently is no support for displaying two digits of counting
  */
  switch (inColorId) {
    case 'S':
      if(red == 99)
        red=0;
      red++;
      break;
    case 'G':
      if(green == 99)
        green=0;
      green++;
      break;
    case  'L':
      if(yellow == 99)
        yellow=0;
      yellow++;
      break;
    case  'B':
      if(purple == 99)
        purple=0;
      purple++;
      break;
    case  'O':
      if(orange == 99)
        orange=0;
      orange++;
      break;
    default:
      return;
      break;
  }
  if(total == 99)
    total=0;
  total++;
}

// name:      lenCheck
// description: helper function for concatColors to concat properly
// args: index var (x), color
// return: none
void lenCheck(int* x, volatile char color){
    char* temp;
    temp = itoaconv((int)color);
    if(color > 9)
    {
        counters[(*x)++] = temp[0];
        counters[(*x)++] = temp[1];
    }
    else{
        counters[(*x)++] = ' ';
        counters[(*x)++] = temp[0];
    }
}

// name: 	  concatColors
// description:	concatenates together the values of all thecounters into a string stored globally
// args: none
// return: none
void concatColors(){
    int x = 0;
    lenCheck(&x, red);
    lenCheck(&x, green);
    lenCheck(&x, yellow);
    lenCheck(&x, purple);
    lenCheck(&x, orange);
    counters[x++] = ' ';
    counters[x++] = ' ';
    lenCheck(&x, total);
    counters[15] = '\0';
}



// name:    setRGB(int r, int g, int b, ReadColor* temp)
// description: sets the color elements of the referenced ReadColor
// args:    3 ints representing r,g and b values, pointer to the read color
// return:  nothing (changes the referenced ReadColor)
void setRGB (volatile int r, volatile int g, volatile int b, ReadColor* temp){
    temp->red = r;
    temp->green = g;
    temp->blue = b;
}

// name:   get<color>()
// description: fetches the 16 bit <color> value from colorsensor.
// args:    none
// return: integer of the upper and lower byte concatenated
// PRECONDITIONS: both i2c on the ChipKIT, and the sensor MUST be configured
//                before calling function
int getRed(){
    int red;

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | RDATAL);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    red = i2c_recv();
    i2c_nak();
    i2c_stop();

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | RDATAH);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    red |= (i2c_recv()<<8);
    i2c_nak();
    i2c_stop();
    return red;
}

int getGreen(){
    int green;

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | GDATAL);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    green = i2c_recv();
    i2c_nak();
    i2c_stop();

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | GDATAH);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    green |= (i2c_recv()<<8);
    i2c_nak();
    i2c_stop();
    return green;
}

int getBlue(){
    int blue;

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | BDATAL);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    blue = i2c_recv();
    i2c_nak();
    i2c_stop();

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | BDATAH);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    blue |= (i2c_recv()<<8);
    i2c_nak();
    i2c_stop();
    return blue;
}

int getClear(){
    int clear;

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | CDATAL);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    clear = i2c_recv();
    i2c_nak();
    i2c_stop();

    do{ // initiate the communication with the sensor in write mode
        i2c_start();
    } while(!i2c_send(SENSOR_ADDR << 1));
    i2c_send(COMMAND_BIT | CDATAH);
    i2c_stop();
    do{ // initiate the communication in read mode
        i2c_start();
    } while(!i2c_send((SENSOR_ADDR << 1) | 1));
    clear |= (i2c_recv()<<8);
    i2c_nak();
    i2c_stop();
    return clear;
}

// name:   rgbTest()
// description: fetches RGB and prints to screen for testing/debugging purpose
// args:    none
// return: void
// PRECONDITIONS: both i2c on the ChipKIT, and the sensor MUST be configured
//                before calling function
void rgbTest(){
    while(1){
        volatile int r = getRed();
        volatile int g = getGreen();
        volatile int b = getBlue();
        display_rgb(&r, &g, &b);
        display_update();
    }
}

void whatColor(ReadColor* incolor){
  //red check
  if(   inRange(incolor->red, RED_R_LOWER, RED_R_UPPER)
     && inRange(incolor->green,RED_G_LOWER, RED_G_UPPER)
     && inRange(incolor->blue, RED_B_LOWER, RED_B_UPPER)){
          incolor->name = "Strawberry!";
        }
  //green check
  else if(   inRange(incolor->red, GREEN_R_LOWER, GREEN_R_UPPER)
          && inRange(incolor->green, GREEN_G_LOWER, GREEN_G_UPPER)
          && inRange(incolor->blue, GREEN_B_LOWER, GREEN_B_UPPER)){
          incolor->name = "Green apple!";
        }
  //yellow check
  else if(   inRange(incolor->red, YELLOW_R_LOWER, YELLOW_R_UPPER)
          && inRange(incolor->green, YELLOW_G_LOWER, YELLOW_G_UPPER)
          && inRange(incolor->blue, YELLOW_B_LOWER, YELLOW_B_UPPER) ){
          incolor->name = "Lemon!";
        }
  //purple check
  else if(   inRange(incolor->red, PURPLE_R_LOWER, PURPLE_R_UPPER)
          && inRange(incolor->green, PURPLE_G_LOWER, PURPLE_G_UPPER)
          && inRange(incolor->blue, PURPLE_B_LOWER, PURPLE_B_UPPER)){
          incolor->name = "Blackcurrant!";
          }
  //orange check
  else if(   inRange(incolor->red, ORANGE_R_LOWER, ORANGE_R_UPPER)
          && inRange(incolor->green, ORANGE_G_LOWER, ORANGE_G_UPPER)
          && inRange(incolor->blue, ORANGE_B_LOWER, ORANGE_B_UPPER)){
          incolor->name = "Orange!";
          }
}

// name:    inRange(volatile int check, int lower, int upper)
// description: checks if first value is within the range of the two other
// args:    check is the var checked for intersection, lower is lower bound, upper is upper bound
// return:  1 or 0 ("true/false")
int inRange(volatile int check, int lower, int upper){
  if(check > lower && check < upper)
    return 1;
  return 0;
}

void counter(){
    ReadColor col;
    //used for interrupt handling
    interrupts = 0;
    inColorId='U';
    char prevColorId='U';

    while(1){
        //collects data
        col.name = "UNKNOWN";
        volatile int r = getRed();
        volatile int g = getGreen();
        volatile int b = getBlue();
        setRGB(r, g, b, &col);
        whatColor(&col);
        T2CON |= (0x1<<15);       //enables/starts timer2 (if it's not running)
        //checks current color with previous
        inColorId = col.name[0];  //(all first characters of flavors are unique) - syntax works

        if( !(prevColorId == inColorId)){ //if the color being read has changed
          timer_reset();
          //resets the timer and interrupt counter since it didn't sense the same color
        }
        prevColorId = inColorId; //sets the previous colorId to the one we just polled

        display_string(0, col.name);
        concatColors(); //no argument because counter-variables are global
        display_string(2, " R G Y P O  Tot");
        display_string(3, counters);
        // display_rgb(&r,&g,&b); // debug
        display_update();

    }
}
