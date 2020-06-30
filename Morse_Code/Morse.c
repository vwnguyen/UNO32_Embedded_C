/**
 * This library implements functions for decoding Morse code. It builds upon the Tree library to use
 * a binary tree data structure for storing the codepoints for all characters. Decoding is then done
 * through simple tree traversal. Additional functionality relies on a 100Hz clock to check the
 * button states for decoding the input of Morse characters through a physical button (BTN4).
 */

// heap size of 4000 bytes
#include <stdio.h>
#include <stdint.h>
#include "BinaryTree.h"
#include "BOARD.h"
#include "Buttons.h"
#include "Morse.h"
#include "stdlib.h"
//const char *input = " EISH54V 3UF    2ARLWP##J#1TNDB6#X##KC##Y##MGZ7#Q##O#8##90";
Node* ROOT1;
Node* TRAVERSAL;

enum {
    WAITING,
    DOT,
    DASH,
    INTER_LETTER
} STATE = WAITING;

/**
 * This function initializes the Morse code decoder. This is primarily the generation of the
 * Morse tree: a binary tree consisting of all of the ASCII alphanumeric characters arranged
 * according to the DOTs and DASHes that represent each character. Traversal of the tree is done by
 * taking the left-child if it is a dot and the right-child if it is a dash. If the morse tree is
 * successfully generated, SUCCESS is returned, otherwise STANDARD_ERROR is returned. This function
 * also initializes the Buttons library so that MorseCheckEvents() can work properly.
 * @return Either SUCCESS if the decoding tree was successfully created or STANDARD_ERROR if not.
 */
int MorseInit(void) {
    ROOT1 = TreeCreate(6, " EISH54V 3UF    2ARL     WP  J 1TNDB6 X  KC  Y  MGZ7 Q  O 8  90");
    if (ROOT1 != NULL) {
        TRAVERSAL = ROOT1;
        return SUCCESS;
    } else {
        return STANDARD_ERROR;
    }
}

int INTER_LETTER_GUARD = FALSE;

/*
 * MorseDecode decodes a Morse string by iteratively being passed MORSE_CHAR_DOT or MORSE_CHAR_DASH.
 * Each call that passes a DOT or DASH returns a SUCCESS if the string could still compose a
 * Morse-encoded character. Passing in the special MORSE_CHAR_END_OF_CHAR constant will terminate
 * decoding and return the decoded character. During that call to MorseDecode() the return value
 * will be the character that was decoded or STANDARD_ERROR if it couldn't be decoded. Another
 * special value exists, MORSE_CHAR_DECODE_RESET, which will clear the stored state. When a
 * MORSE_CHAR_DECODE_RESET is done, SUCCESS will be returned. If the input is not a valid MorseChar
 * then the internal state should be reset and STANDARD_ERROR should be returned.
 * 
 * @param in A value from the MorseChar enum which specifies how to traverse the Morse tree.
 * 
 * @return Either SUCCESS on DECODE_RESET or when the next traversal location is still a valid
 *         character, the decoded character on END_OF_CHAR, or STANDARD_ERROR if the Morse tree
 *         hasn't been initialized, the next traversal location doesn't exist/represent a character,
 *         or `in` isn't a valid member of the MorseChar enum.
 */
char MorseDecode(MorseChar in) {
    if (ROOT1 == NULL || (in != '.' && in != '-' && in != '\0' && in != '#')) {
        return STANDARD_ERROR;
    }

    //    static Node* TRAVERSAL = ROOT1;
    if (in == MORSE_CHAR_END_OF_CHAR) {
        if (TRAVERSAL->data == ' ') {
            return '#';
        } else {
            return TRAVERSAL->data;
        }
    } else if (in == MORSE_CHAR_DOT) {
        TRAVERSAL = TRAVERSAL->leftChild;
        if (TRAVERSAL == NULL) {
            TRAVERSAL = ROOT1;
            return STANDARD_ERROR;
        }
        return SUCCESS;
    } else if (in == MORSE_CHAR_DASH) {
        TRAVERSAL = TRAVERSAL->rightChild;
        if (TRAVERSAL == NULL) {
            TRAVERSAL = ROOT1;
            return STANDARD_ERROR;
        }
        return SUCCESS;
    } else {
        TRAVERSAL = ROOT1;
        return SUCCESS;
    }

}

/**
 * This function calls ButtonsCheckEvents() once per call and returns which, if any,
 * of the Morse code events listed in the enum above have been encountered. It checks for BTN4
 * events in its input and should be called at 100Hz so that the timing works. The
 * length that BTN4 needs to be held down for a dot is >= 0.25s and < 0.50s with a dash being a button
 * down event for >= 0.5s. The button uptime various between dots/dashes (>= .5s), letters
 * (>= 1s), and words (>= 2s).
 *
 * @note This function assumes that the buttons are all unpressed at startup, so that the first
 *       event it will see is a BUTTON_EVENT_*DOWN.
 *
 * So pressing the button for 0.3s, releasing it for 0.3s, pressing it for 0.6s, and then waiting
 * will decode the string '.-' (A). It will trigger the following order of events:
 * MORSE_EVENT_NONEs, 1 MORSE_EVENT_DOT, more MORSE_EVENT_NONEs, a MORSE_EVENT_DASH, more
 * MORSE_EVENT_NONEs, a MORSE_EVENT_END_CHAR, and then MORSE_EVENT_INTER_WORDs.
 * 
 * @return The MorseEvent that occurred.
 */
MorseEvent MorseCheckEvents(void) {
    static int eventTicker = 0;
    static int buttonCheck;
    static int morseEvent;
    eventTicker++;
    buttonCheck = ButtonsCheckEvents();
    switch (STATE) {
        case WAITING:
            if (buttonCheck & BUTTON_EVENT_4DOWN) {
                STATE = DOT;
                eventTicker = 0;
                //return MORSE_EVENT_NONE;
                morseEvent = MORSE_EVENT_NONE;
            } else {
                STATE = WAITING;
                morseEvent = MORSE_EVENT_NONE;
            }
            break;
        case DOT:
            if (buttonCheck & BUTTON_EVENT_4UP) {
                eventTicker = 0;
                STATE = INTER_LETTER;
                morseEvent = MORSE_EVENT_DOT;
            } else if (eventTicker >= MORSE_EVENT_LENGTH_DOWN_DOT) {
                STATE = DASH;
                morseEvent = MORSE_EVENT_NONE;
            } else {
                STATE = DOT;
                morseEvent = MORSE_EVENT_NONE;
            }

            break;
        case DASH:
            if (buttonCheck & BUTTON_EVENT_4UP) {
                eventTicker = 0;
                STATE = INTER_LETTER;
                morseEvent = MORSE_EVENT_DASH;
            } else {
                STATE = DASH;
                morseEvent = MORSE_EVENT_NONE;
            }
            break;
        case INTER_LETTER:
            if (eventTicker >= MORSE_EVENT_LENGTH_UP_INTER_LETTER_TIMEOUT) {
                STATE = WAITING;
                morseEvent = MORSE_EVENT_INTER_WORD;
            } else if ((buttonCheck & BUTTON_EVENT_4DOWN)) {
                if (eventTicker >= MORSE_EVENT_LENGTH_UP_INTER_LETTER) {
                    INTER_LETTER_GUARD = TRUE;
                } else {
                    INTER_LETTER_GUARD = FALSE;
                }
                if (INTER_LETTER_GUARD == TRUE) {
                    eventTicker = 0;
                    STATE = DOT;
                    morseEvent = MORSE_EVENT_INTER_LETTER;
                } else {
                    eventTicker = 0;
                    STATE = DOT;
                    morseEvent = MORSE_EVENT_NONE;
                }
            } else {
                morseEvent = MORSE_EVENT_NONE;
            }
    }
    return morseEvent;
    //morseEvent = MORSE_EVENT_NONE;
}
