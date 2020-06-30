
#include "Agent.h"
#include "Protocol.h"
#include "Field.h"
#include "string.h"
#include "Uart1.h"
#include "Oled.h"
#include "OledDriver.h"
#include "FieldOled.h"
#include "stdio.h"

typedef enum {
    GUESS_UNKNOWN,
    GUESS_DIRECTION,
    FINISH

} GUESS_STATE;


GUESS_STATE AGENT_GUESS = GUESS_UNKNOWN;
AgentState STATE = AGENT_STATE_GENERATE_NEG_DATA;
Field myField;
Field enemyField;
GuessData Guess, OppGuess;
NegotiationData NegotiateData, OppData;
FieldOledTurn turn;
/**
 * The following constants describes strings that should be output onto the OLED
 * when errors occur:
 *   * AGENT_ERROR_STRING_NEG_DATA: Displayed when the negotiation data from the opponent didn't
 *                                     validate, as in they cheated.
 *   * AGENT_ERROR_STRING_PARSING: Displayed whenever a message fails to parse. This should happen
 *                                 very rarely, so we treat it as a game-ending fatal error.
 *   * AGENT_ERROR_STRING_ORDERING: Displayed when turn ordering results in a tie. Also a very rare
 *                                  occurance, so treated as a fatal error.
 */
#define AGENT_ERROR_STRING_NEG_DATA "Received invalid\nnegotiation data"
#define AGENT_ERROR_STRING_PARSING     "Message parsing\nfailed"
#define AGENT_ERROR_STRING_ORDERING    "Turn ordering\nfailed"

static void errorNegotiateMessage(void);
static void errorStringOrderingMessage(void);
static void errorStringParsingMessage(void);

/**
 * The Init() function for an Agent sets up everything necessary for an agent before the game
 * starts. This can include things like initialization of the field, placement of the boats,
 * etc. The agent can assume that stdlib's rand() function has been seeded properly in order to
 * use it safely within.
 */
void AgentInit(void)
{
    FieldInit(&myField, FIELD_POSITION_EMPTY);
    FieldInit(&enemyField, FIELD_POSITION_UNKNOWN);
    int randCol = (rand()) % FIELD_COLS;
    int randRow = (rand()) % FIELD_ROWS;
    //int randBoat = (rand() + 1) % 4;
    int randDirection = (rand()) % 4;
    // get a valid field first using random values

    int goodSmallBoatAdd = FieldAddBoat(&myField, randCol, randRow, randDirection, FIELD_BOAT_SMALL);
    while (goodSmallBoatAdd != TRUE) {
        randCol = (rand()) % FIELD_COLS;
        randRow = (rand()) % FIELD_ROWS;
        //int randBoat = (rand() + 1) % 4;
        randDirection = (rand()) % 4;
        if (randDirection == FIELD_BOAT_DIRECTION_NORTH) {
            // if the direction is north we have to limit the row range
            while (randRow < 2 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_SOUTH) {
            // if the direction is north we have to limit the row range
            while (randRow > 3 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_WEST) {
            // if the direction is north we have to limit the row range
            while (randCol < 2 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_COLS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_EAST) {
            // if the direction is north we have to limit the row range
            while (randCol > 7 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_ROWS;
            }
        }
        goodSmallBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_SMALL);
    }

    // ADDING A RANDOM MEDIUM BOAT
    int goodMedBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_MEDIUM);
    while (goodMedBoatAdd != TRUE) {
        randCol = (rand()) % FIELD_COLS;
        randRow = (rand()) % FIELD_ROWS;
        //int randBoat = (rand() + 1) % 4;
        randDirection = (rand()) % 4;
        if (randDirection == FIELD_BOAT_DIRECTION_NORTH) {
            // if the direction is north we have to limit the row range
            while (randRow < 3 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_SOUTH) {
            // if the direction is north we have to limit the row range
            while (randRow > 2 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_WEST) {
            // if the direction is north we have to limit the row range
            while (randCol < 3 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_COLS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_EAST) {
            // if the direction is north we have to limit the row range
            while (randCol > 6 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_ROWS;
            }
        }
        goodMedBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_MEDIUM);
    }

    randCol = (rand()) % FIELD_COLS;
    randRow = (rand()) % FIELD_ROWS;
    //int randBoat = (rand() + 1) % 4;
    randDirection = (rand()) % 4;

    int goodLargeBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_LARGE);
    while (goodLargeBoatAdd != TRUE) {
        randCol = (rand()) % FIELD_COLS;
        randRow = (rand()) % FIELD_ROWS;
        //int randBoat = (rand() + 1) % 4;
        randDirection = (rand()) % 4;
        if (randDirection == FIELD_BOAT_DIRECTION_NORTH) {
            // if the direction is north we have to limit the row range
            while (randRow < 4 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_SOUTH) {
            // if the direction is north we have to limit the row range
            while (randRow > 1 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_WEST) {
            // if the direction is north we have to limit the row range
            while (randCol < 4 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_COLS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_EAST) {
            // if the direction is north we have to limit the row range
            while (randCol > 5 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_ROWS;
            }
        }
        goodLargeBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_LARGE);
    }

    randCol = (rand()) % FIELD_COLS;
    randRow = (rand()) % FIELD_ROWS;
    //int randBoat = (rand() + 1) % 4;
    randDirection = (rand()) % 4;

    int goodHugeBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_HUGE);
    while (goodHugeBoatAdd != TRUE) {
        randCol = (rand()) % FIELD_COLS;
        randRow = (rand()) % FIELD_ROWS;
        //int randBoat = (rand() + 1) % 4;
        randDirection = (rand()) % 4;
        if (randDirection == FIELD_BOAT_DIRECTION_NORTH) {
            // if the direction is north we have to limit the row range
            while (randRow < 5 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_SOUTH) {
            // if the direction is north we have to limit the row range
            while (randRow > 0 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randRow = (rand()) % FIELD_ROWS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_WEST) {
            // if the direction is north we have to limit the row range
            while (randCol < 5 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_COLS;
            }
        }
        if (randDirection == FIELD_BOAT_DIRECTION_EAST) {
            // if the direction is north we have to limit the row range
            while (randCol > 4 || FieldAt(&myField, randRow, randCol) != FIELD_POSITION_EMPTY) {
                randCol = (rand()) % FIELD_ROWS;
            }
        }
        goodHugeBoatAdd = FieldAddBoat(&myField, randRow, randCol, randDirection, FIELD_BOAT_HUGE);
    }

}

/**
 * The Run() function for an Agent takes in a single character. It then waits until enough
 * data is read that it can decode it as a full sentence via the Protocol interface. This data
 * is processed with any output returned via 'outBuffer', which is guaranteed to be 255
 * characters in length to allow for any valid NMEA0183 messages. The return value should be
 * the number of characters stored into 'outBuffer': so a 0 is both a perfectly valid output and
 * means a successful run.
 * @param in The next character in the incoming message stream.
 * @param outBuffer A string that should be transmit to the other agent. NULL if there is no
 *                  data.
 * @return The length of the string pointed to by outBuffer (excludes \0 character).
 */
int AgentRun(char in, char *outBuffer)
{
    int outBufferLength = 0;
    outBuffer = 0;
    char challengeMessage[PROTOCOL_MAX_MESSAGE_LEN];
    char detMessage[PROTOCOL_MAX_MESSAGE_LEN];
    char hitmessage[PROTOCOL_MAX_MESSAGE_LEN];
    char coomessage[PROTOCOL_MAX_MESSAGE_LEN];
    ProtocolParserStatus check;
    int rowGuess = rand() % FIELD_ROWS;
    int colGuess = rand() % FIELD_COLS;
    static int initialrowHit;
    static int initialcolHit;
    static int directionCounter = 0;
    static int hitflag = 0;
    int j;
    float T1;

    switch (STATE) {

    case AGENT_STATE_INVALID:
        break;
    case AGENT_STATE_GENERATE_NEG_DATA:
        ProtocolGenerateNegotiationData(&NegotiateData);
        outBufferLength = ProtocolEncodeChaMessage(challengeMessage, &NegotiateData);
        outBuffer = challengeMessage;
        STATE = AGENT_STATE_SEND_CHALLENGE_DATA;
        puts("sent the chaMessage");
        break;

    case AGENT_STATE_SEND_CHALLENGE_DATA:
        //            while (i < 3) {
        //                c = decodedata.message[i];
        //                msgcheck[i] = c;
        //                i++;
        //            }
        //            msgcheck[3] = '\0';
        if (in != NULL) {

            check = ProtocolDecode(in, &OppData, &Guess);
            if (check == PROTOCOL_PARSING_FAILURE) {
                STATE = AGENT_STATE_INVALID;
                errorNegotiateMessage();
            } else if (check == PROTOCOL_PARSED_CHA_MESSAGE) {
                outBufferLength = ProtocolEncodeDetMessage(detMessage, &NegotiateData);
                outBuffer = detMessage;
                STATE = AGENT_STATE_DETERMINE_TURN_ORDER;
                puts("sent the determine detMessage");
            }
        }
        break;
    case AGENT_STATE_DETERMINE_TURN_ORDER:
        if (in != NULL) {
            check = ProtocolDecode(in, &OppData, &Guess);
            if (check == PROTOCOL_PARSING_FAILURE) {
                STATE = AGENT_STATE_INVALID;
                errorNegotiateMessage();
            } else if (check == PROTOCOL_PARSED_DET_MESSAGE) {
                puts("RECEIVED DET");
                if (ProtocolValidateNegotiationData(&OppData)) {
                    puts("Validated");
                    switch (ProtocolGetTurnOrder(&NegotiateData, &OppData)) {
                    case TURN_ORDER_TIE:
                        errorStringOrderingMessage();
                        STATE = AGENT_STATE_INVALID;
                        break;
                    case TURN_ORDER_START:
                        turn = FIELD_OLED_TURN_MINE;
                        FieldOledDrawScreen(&myField, &enemyField, turn);
                        OledUpdate();
                        STATE = AGENT_STATE_SEND_GUESS;
                        break;
                    case TURN_ORDER_DEFER:
                        turn = FIELD_OLED_TURN_THEIRS;
                        FieldOledDrawScreen(&myField, &enemyField, turn);
                        OledUpdate();
                        STATE = AGENT_STATE_WAIT_FOR_GUESS;
                        break;
                    }
                } else {
                    puts("invalid negotiate data FROM DETERMINE TURN ORDER");
                    STATE = AGENT_STATE_INVALID;
                    errorNegotiateMessage();
                }
            }
        }
        break;
    case AGENT_STATE_WAIT_FOR_GUESS:
        if (in != NULL) {
            check = ProtocolDecode(in, &OppData, &OppGuess);
            printf("%c %d\n",in,check);
            if (check == PROTOCOL_PARSING_FAILURE) {
                STATE = AGENT_STATE_INVALID;
                errorStringParsingMessage();
                return outBufferLength;
            } else if (check == PROTOCOL_PARSED_COO_MESSAGE) {
                puts("received coo message sending our own");
                FieldRegisterEnemyAttack(&myField, &OppGuess);
                if (AgentGetStatus()) {
                    turn = FIELD_OLED_TURN_MINE;
                    outBufferLength = ProtocolEncodeHitMessage(hitmessage, &OppGuess);
                    outBuffer = hitmessage;
                    FieldOledDrawScreen(&myField, &enemyField, turn);
                    STATE = AGENT_STATE_SEND_GUESS;
                } else {
                    puts("received message, we lost");
                    puts("now we gonna go in the lost state");
                    turn = FIELD_OLED_TURN_NONE;
                    outBufferLength = ProtocolEncodeHitMessage(hitmessage, &OppGuess);
                    outBuffer = hitmessage;
                    FieldOledDrawScreen(&myField, &enemyField, turn);
                    STATE = AGENT_STATE_LOST;
                }
            }
        }
        break;
    case AGENT_STATE_SEND_GUESS:
        T1 = BOARD_GetPBClock()/8;
        for (j = 0; j < T1; j++ ){
            
        }
        switch (AGENT_GUESS) {
        case GUESS_UNKNOWN:
            if (Guess.hit == HIT_MISS) {
                while (FieldAt(&enemyField, rowGuess, colGuess) != FIELD_POSITION_UNKNOWN) {
                    rowGuess = rand() % FIELD_ROWS;
                    colGuess = rand() % FIELD_COLS;
                }
                Guess.row = rowGuess;
                Guess.col = colGuess;
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            } else if (Guess.hit == HIT_HIT) {
                AGENT_GUESS = GUESS_DIRECTION;
                initialrowHit = Guess.row;
                initialcolHit = Guess.col;
            }
            break;
        case GUESS_DIRECTION:
            // FIRST TRY NORTH
            if (hitflag == 2) {
                AGENT_GUESS = FINISH;
                hitflag = 0;
                break;
            }
            if (directionCounter == FIELD_BOAT_DIRECTION_NORTH) {
                Guess.row = initialrowHit - 1;
                directionCounter++;
                if (FieldAt(&enemyField, rowGuess, colGuess) != FIELD_POSITION_UNKNOWN) {
                    break;
                }
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            // EAST  
            if (directionCounter == FIELD_BOAT_DIRECTION_EAST) {
                Guess.col = initialcolHit + 1;
                directionCounter++;
                if (FieldAt(&enemyField, rowGuess, colGuess) != FIELD_POSITION_UNKNOWN) {
                    break;
                }
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            if (directionCounter == FIELD_BOAT_DIRECTION_SOUTH) {
                Guess.row = initialrowHit + 1;
                directionCounter++;
                if (FieldAt(&enemyField, rowGuess, colGuess) != FIELD_POSITION_UNKNOWN) {
                    break;
                }
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            if (directionCounter == FIELD_BOAT_DIRECTION_WEST) {
                Guess.col = initialcolHit - 1;
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            // break for the guess switch
            break;
        case FINISH:

            if (Guess.hit == HIT_SUNK_HUGE_BOAT || Guess.hit == HIT_SUNK_LARGE_BOAT
                    || Guess.hit == HIT_SUNK_MEDIUM_BOAT || Guess.hit == HIT_SUNK_SMALL_BOAT) {
                AGENT_GUESS = GUESS_UNKNOWN;
                directionCounter = 0;
                break;
            }
            if (Guess.hit == HIT_MISS) {
                AGENT_GUESS = GUESS_DIRECTION;
                break;
            }
            if (directionCounter == FIELD_BOAT_DIRECTION_NORTH) {
                Guess.row -= 1;
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            // EAST  
            if (directionCounter == FIELD_BOAT_DIRECTION_EAST) {
                Guess.col += 1;
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            if (directionCounter == FIELD_BOAT_DIRECTION_SOUTH) {
                Guess.row += 1;
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
            }
            if (directionCounter == FIELD_BOAT_DIRECTION_WEST) {
                Guess.col -= 1;
                outBufferLength = ProtocolEncodeCooMessage(coomessage, &Guess);
                outBuffer = coomessage;
                STATE = AGENT_STATE_WAIT_FOR_HIT;
                //directionCounter = 0;
            }
            break;
        }
        // send guess break
        break;

    case AGENT_STATE_WAIT_FOR_HIT:
        if (in != NULL && AgentGetEnemyStatus() == 0) {
            puts("received message, we won");
            check = ProtocolDecode(in, &OppData, &Guess);
            if (check == PROTOCOL_PARSING_FAILURE) {
                STATE = AGENT_STATE_INVALID;
                errorStringParsingMessage();
                return outBufferLength;
            } else if (check == PROTOCOL_PARSED_HIT_MESSAGE) {
                puts("now we gonna go in the win state");
                turn = FIELD_OLED_TURN_NONE;
                FieldOledDrawScreen(&myField, &enemyField, turn);
                STATE = AGENT_STATE_WON;
            }
        } else if (in != NULL && AgentGetEnemyStatus() != 0) {  
            check = ProtocolDecode(in, &OppData, &Guess);                      
            printf("%c %d\n",in,check);
            if (Guess.hit) {
                hitflag++;
            }
            if (check == PROTOCOL_PARSING_FAILURE) {
                STATE = AGENT_STATE_INVALID;
                errorStringParsingMessage();
                return outBufferLength;
            } else if (check == PROTOCOL_PARSED_HIT_MESSAGE) {
                FieldUpdateKnowledge(&enemyField, &Guess);
                turn = FIELD_OLED_TURN_THEIRS;
                FieldOledDrawScreen(&myField, &enemyField, turn);
                STATE = AGENT_STATE_WAIT_FOR_GUESS;
            }
        }
        // wait for hit break
        break;
    case AGENT_STATE_WON:
        break;
    case AGENT_STATE_LOST:
        break;
    }
    OledUpdate();
    return outBufferLength;

}

/**
 * StateCheck() returns a 4-bit number indicating the status of that agent's ships. The smallest
 * ship, the 3-length one, is indicated by the 0th bit, the medium-length ship (4 tiles) is the
 * 1st bit, etc. until the 3rd bit is the biggest (6-tile) ship. This function is used within
 * main() to update the LEDs displaying each agents' ship status. This function is similar to
 * Field::FieldGetBoatStates().
 * @return A bitfield indicating the sunk/unsunk status of each ship under this agent's control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetStatus(void)
{
    uint8_t boatStatus = FieldGetBoatStates(&myField);
    return boatStatus;
}

/**
 * This function returns the same data as `AgentCheckState()`, but for the enemy agent.
 * @return A bitfield indicating the sunk/unsunk status of each ship under the enemy agent's
 *         control.
 *
 * @see Field.h:FieldGetBoatStates()
 * @see Field.h:BoatStatus
 */
uint8_t AgentGetEnemyStatus(void)
{
    uint8_t boatStatus = FieldGetBoatStates(&enemyField);
    return boatStatus;
}

static void errorNegotiateMessage(void)
{
    OledDrawString("                             ");
    OledDrawString("\n                             ");
    OledDrawString("\n\n                             ");
    OledDrawString("\n\n\n                             ");
    OledDrawString("\n\n\n\n                             ");
    OledDrawString(AGENT_ERROR_STRING_NEG_DATA);
    OledUpdate();
}

static void errorStringOrderingMessage(void)
{
    OledDrawString("                             ");
    OledDrawString("\n                             ");
    OledDrawString("\n\n                             ");
    OledDrawString("\n\n\n                             ");
    OledDrawString("\n\n\n\n                             ");
    OledDrawString(AGENT_ERROR_STRING_ORDERING);
    OledUpdate();
}

static void errorStringParsingMessage(void)
{
    OledDrawString("                             ");
    OledDrawString("\n                             ");
    OledDrawString("\n\n                             ");
    OledDrawString("\n\n\n                             ");
    OledDrawString("\n\n\n\n                             ");
    OledDrawString(AGENT_ERROR_STRING_PARSING);
    OledUpdate();
}




