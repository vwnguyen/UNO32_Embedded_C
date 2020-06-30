// **** Include libraries here ****
// Standard C libraries


//CMPE13 Support Library
#include "BOARD.h"
#include "stdio.h"
// Microchip libraries
#include <xc.h>
#include <plib.h>
#include "Morse.h"
#include "BinaryTree.h"

// User libraries


// **** Set any macros or preprocessor directives here ****
// Specify a bit mask for setting/clearing the pin corresponding to BTN4. Should only be used when
// unit testing the Morse event checker.
#define BUTTON4_STATE_FLAG (1 << 7)

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
MorseEvent morseEvent;
// **** Declare any function prototypes here ****

int main() {
    BOARD_Init();


    // Configure Timer 2 using PBCLK as input. We configure it using a 1:16 prescalar, so each timer
    // tick is actually at F_PB / 16 Hz, so setting PR2 to F_PB / 16 / 100 yields a .01s timer.
    OpenTimer2(T2_ON | T2_SOURCE_INT | T2_PS_1_16, BOARD_GetPBClock() / 16 / 100);

    // Set up the timer interrupt with a medium priority of 4.
    INTClearFlag(INT_T2);
    INTSetVectorPriority(INT_TIMER_2_VECTOR, INT_PRIORITY_LEVEL_4);
    INTSetVectorSubPriority(INT_TIMER_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
    INTEnable(INT_T2, INT_ENABLED);

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    int goodInit = MorseInit();
    printf("good initialization? %d\n", goodInit);
    //int i = 0;
    //char TESTARRAY[] = {'.','-','#','\0'}; 
    // U TEST
    MorseDecode('.');
    MorseDecode('.');
    MorseDecode('-');
    printf("Should result in U: ");
    printf("%c\n", MorseDecode('#'));
    MorseDecode('\0');
    // W TEST
    MorseDecode('.');
    MorseDecode('-');
    MorseDecode('-');
    printf("Should result in W: ");
    printf("%c\n", MorseDecode('#'));
    MorseDecode('\0');
    // NULL TEST
    MorseDecode('.');
    MorseDecode('.');
    MorseDecode('-');
    MorseDecode('-');
    printf("Current char is NULL: ");
    printf("%c\n", MorseDecode('#'));
    MorseDecode('\0');
    //    	MORSE_EVENT_NONE,
    //	MORSE_EVENT_DOT,
    //	MORSE_EVENT_DASH,
    //	MORSE_EVENT_INTER_LETTER,
    //	MORSE_EVENT_INTER_WORD
    while (1) {
        if (morseEvent) {
            if (morseEvent & MORSE_EVENT_NONE) {
                puts("MORSE EVENT NONE");
            }
            if (morseEvent & MORSE_EVENT_DOT) {
                puts("MORSE EVENT DOT");
            }
            if (morseEvent & MORSE_EVENT_DASH) {
                puts("MORSE EVENT DASH");
            }
            if (morseEvent & MORSE_EVENT_INTER_LETTER) {
                puts("MORSE EVENT INTER LETTER");
            }
            if (morseEvent & MORSE_EVENT_INTER_WORD) {
                puts("MORSE EVENT INTER WORD");
            }
            morseEvent = MORSE_EVENT_NONE;
        }
    }
    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks.
     *****************************************************************************/
    while (1);
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;
    //******** Put your code here *************//
    morseEvent = MorseCheckEvents();
}
