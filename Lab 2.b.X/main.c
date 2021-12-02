#include <xc.h>
#include "func.h"
#include <stdio.h>
#include <stdlib.h>

#pragma config JTAGEN = OFF
#pragma config FWDTEN = OFF
#pragma config FNOSC = FRCPLL
#pragma config FSOSCEN = OFF
#pragma config POSCMOD = EC
#pragma config OSCIOFNC = ON
#pragma config FPBDIV = DIV_1
#pragma config FPLLIDIV = DIV_2
#pragma config FPLLMUL = MUL_20
#pragma config FPLLODIV = DIV_1

void main() {
    TRISFbits.TRISF3 = 1; // RF3 (SW0) configured as input
    TRISFbits.TRISF5 = 1; // RF5 (SW1) configured as input
    TRISBbits.TRISB9 = 1; // RB9 (SW7) configured as input
    ANSELBbits.ANSB9 = 0; // RB9 (SW7) disabled analog
    TRISBbits.TRISB14 = 0; //buzzer configured as output
    ANSELBbits.ANSB14 = 0; //buzzer disabled analog

    setup();
    sw7();
    while (1) {
        sw1();
        if (PORTFbits.RF3) // sw0 on?
            sw0();
        if(PORTBbits.RB9) //sw7 on?
            sw7();
    }
}