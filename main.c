///*==========================================================*/
/*  File: main.c
/*  Authors: Martin Falke & Ludvig Christensen
/*  Written: 2017
/*  Most recent update: 2017-02-26 by LC
/*
/*  This file contains the code that is run at the startup of the program.
/*  It makes sure that the initialization code is run [ init() ] and that the program
/*  continues with updated values and visual representation on the ChipKIT
/*  display [ update() ].
boiii
///*==========================================================*/

#include <pic32mx.h>
#include "protocol.h"
#include "mipslab.h"
#include "proj_defs.h"

int main(int argc, char const *argv[]) {
    init();
    //rgbTest();
    counter();
    // display_image(96, skittles);
    return 0;
}
