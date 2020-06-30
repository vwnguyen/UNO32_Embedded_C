// **** Include libraries here ****
// Standard C libraries


//CMPE13 Support Library
#include "BOARD.h"
#include "stdio.h"
#include "string.h"
// Microchip libraries
#include <xc.h>
#include <plib.h>

// User libraries
#include "BinaryTree.h"
#include "Morse.h"
#include "Oled.h"

// **** Set any macros or preprocessor directives here ****
// Specify a bit mask for setting/clearing the pin corresponding to BTN4. Should only be used when
// unit testing the Morse event checker.
#define BUTTON4_STATE_FLAG (1 << 7)

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****
MorseEvent morseEvent;
static char topline[20] = "";
static char botline[100] = "\n";
// **** Declare any function prototypes here ****
static void clearOLED(void);
static void oledAppendTop(char in);
static void oledAppendBottom(char in, int flag);
char in;
char decoded;
int newWord;
int traversalCount;
int walkOffFlag = 0;
#define CLEARTOP "                     "

int main() {
    BOARD_Init();
    OledInit();
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
     ****************************************************************************/
    int goodInit = MorseInit();
    if (goodInit != 1) {
        printf("You messed up bucko, MorseInit() failed\n");
        FATAL_ERROR();
    }
    while (1) {
        newWord = 0;
        if (morseEvent) {
            if (morseEvent == MORSE_EVENT_DOT) {
                traversalCount++;
                puts("MORSE EVENT DOT");
                in = '.';
                if (MorseDecode(in) == STANDARD_ERROR) {
                    clearOLED();
                    walkOffFlag++;
                } else {
                    oledAppendTop(in);
                    walkOffFlag = 0;
                }
            }
            if (morseEvent == MORSE_EVENT_DASH) {
                //traversalCount++;
                puts("MORSE EVENT DASH");
                in = '-';
                if (MorseDecode(in) == STANDARD_ERROR) {
                    clearOLED();
                    walkOffFlag++;
                } else {
                    oledAppendTop(in);
                    walkOffFlag = 0;

                }
            }
            if (morseEvent == MORSE_EVENT_INTER_LETTER) {
                puts("MORSE EVENT INTER LETTER");
                // if we wander to the bottom of the tree make the decode a #
                decoded = MorseDecode('#');
                if (walkOffFlag == 0) {
                    oledAppendBottom(decoded, newWord);
                } else {
                    walkOffFlag = 0;
                }
                clearOLED();
                MorseDecode('\0');
            }
            if (morseEvent == MORSE_EVENT_INTER_WORD) {
                puts("MORSE EVENT INTER WORD");
                newWord++;
                decoded = MorseDecode('#');
                clearOLED();
                if (walkOffFlag == 0) {
                    oledAppendBottom(decoded, newWord);
                } else {
                    walkOffFlag = 0;
                }
                MorseDecode('\0');
            }
            morseEvent = MORSE_EVENT_NONE;
        }
    }
    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks.
     *****************************************************************************/
    while (1);
}

static void clearOLED(void) {
    strcpy(topline, "");
    OledDrawString(CLEARTOP);
    OledUpdate();
}

static void oledAppendTop(char in) {
    strcat(topline, &in);
    OledDrawString(topline);
    OledUpdate();
}

static void oledAppendBottom(char in, int newWord) {
    // append the bottom depending on whether we need a space 
    // for a new word or not
    if (newWord == 0) {
        strcat(botline, &in);
        OledDrawString(botline);
        OledUpdate();
    } else {
        char space[] = " ";
        strcat(botline, &in);
        strcat(botline, space);
        OledDrawString(botline);
        OledUpdate();
    }
}

void __ISR(_TIMER_2_VECTOR, IPL4AUTO) TimerInterrupt100Hz(void) {
    // Clear the interrupt flag.
    IFS0CLR = 1 << 8;
    //******** Put your code here *************//
    morseEvent = MorseCheckEvents();
}
