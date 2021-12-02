#include "func.h"
#include <string.h>
#include <xc.h>
int shiftCounter = 0;
int ddramIndex = 0;
int previousLine = 0;

void sendLCDPulse() {
    PORTDbits.RD4 = 1; // Pulse
    PORTDbits.RD4 = 0;
    busy();
}

void setup() {
    TRISBbits.TRISB15 = 0; // RB15 (DISP_RS) set as an output
    ANSELBbits.ANSB15 = 0; // disable analog functionality on RB15 (DISP_RS)
    TRISDbits.TRISD5 = 0; // RD5 (DISP_RW) set as an output
    TRISDbits.TRISD4 = 0; // RD4 (DISP_EN) set as an output
    TRISE &= 0xff00;
    ANSELEbits.ANSE2 = 0;
    ANSELEbits.ANSE4 = 0;
    ANSELEbits.ANSE5 = 0;
    ANSELEbits.ANSE6 = 0;
    ANSELEbits.ANSE7 = 0;
    PORTBbits.RB15 = 0; // set RS to receive instructions
    PORTDbits.RD5 = 0; // set to write mode

    char instructions[] = {0x38, 0x38, 0x38, 0xe, 0x6, 0x1, 0x40};
    // 0x40 is for new font configuration in CGRAM.

    for (int i = 0; i < 7; i++) { // transfer instructions
        PORTE = instructions[i];
        // falling edge -> write data:
        sendLCDPulse();
    }

    char CG_newFont[8] = {0x0E, 0x11, 0x10, 0x10, 0x1F, 0x1B, 0x1B, 0x1F}; // open lock icon
    char locks[1] = {0};

    PORTBbits.RB15 = 1; // set RS to input
    for (int i = 0; i < 8; i++) {
        PORTE = CG_newFont[i];
        sendLCDPulse();
    }

    //writing the new font to the screen
    PORTBbits.RB15 = 0; //rs control 
    PORTE = 0x80; //DDRAM
    sendLCDPulse();
    PORTBbits.RB15 = 1; //rs 

    for (int i = 0; i < 1; i++) {
        PORTE = locks[i];
        sendLCDPulse();
    }
}

void busy(int value) {
    if (value <= 0) {
        value = 1;
    }
    int j;
    for (j = 0; j <= 16000 * value; j++);
}

void sw0() {
    PORTBbits.RB15 = 0; //DISP_RS -> Low for instruction Transfer
    PORTDbits.RD5 = 0; //DISP_RW -> Low for write mode
    PORTE = 0b11100;
    sendLCDPulse();
    busy(5);
}

void sw7() {
    int j;
    for (j = 0; j <= 100000; j++) {
        if (j % 440 == 0) {
            buzzer();
        }
    }
}

void buzzer() {
    LATBbits.LATB14 ^= 1; //XOR 
}

void sw1() {
    if (previousLine != PORTFbits.RF5) {
        previousLine = PORTFbits.RF5;
        clearLCD();
        int line = PORTFbits.RF5; // sw1 on/off determined line
        PORTBbits.RB15 = 0; // transferring instruction in order to write further commands later on
        if (line == 0) { //0x80 - first line, 0xc0 - second line
            PORTE = 0x80;
        } else {
            PORTE = 0xc0;
        }
        sendLCDPulse(); // Pulse - Whenever transferring a new command we pulse (=refresh)
        PORTBbits.RB15 = 1; // RS = 1 transferring data - what we want the LCD to display
        PORTDbits.RD5 = 0; // Write 
        PORTE = ddramIndex;
        sendLCDPulse();
    }
}

void clearLCD() {
    PORTBbits.RB15 = 0; //DISP_RS -> Low for instruction Transfer
    PORTDbits.RD5 = 0; //DISP_RW -> Low for write mode
    PORTE = 0b1;
    sendLCDPulse();
}