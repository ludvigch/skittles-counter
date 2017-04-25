# skittles-counter

A project for the IS1200 course in computer hardware engineering at KTH. 

The objective of the project was to create a “skittles counter” that keeps track of skittles and their respective colors using an external color sensor. 

The project is based around the ChipKIT Uno32 but also uses the basic I/O shield and the TCS34725 color sensor. It also utilizes a breadboard and some wiring to connect the sensor to the I/O shield’s I2C connection pins and to supply power for the sensor’s LED (used to improve light conditions).

Compile using the mcb32 toolchain(https://github.com/is1200-example-projects/mcb32tools):
1. Enter compilation environment:
    user@computer:~/skittles-counter$ . /opt/mcb32tools/environment
2. Compile the project:
    user@computer:~/skittles-counter$ make
3. Flash to the chipkit board
    user@computer:~/skittles-counter$ make install TTYDEV=/dev/path-to-serial-port
    
