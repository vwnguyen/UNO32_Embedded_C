#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GenericTypeDefs.h>
#include "Protocol.h"

static uint8_t ASCIItoHEX(char thing)
{
    if (48 <= thing && thing <= 57) {
        return thing - 48;
    } else if (65 <= thing && thing <= 70) {
        return thing - 55;
    } else if (97 <= thing && thing <= 102) {
        return thing - 87;
    } else {
        return -1;
    }
}

enum Decode {
    WAITING, RECORDING, FIRST_CHECKSUM_HALF, SECOND_CHECKSUM_HALF, NEWLINE
} Decode = WAITING;

typedef struct DecodeData {
    enum Decode check;
    char message[PROTOCOL_MAX_MESSAGE_LEN];
    int index;
    uint8_t checksum;
} DecodeData;

DecodeData decodedata;

static uint8_t checksummer(char *thing)
{
    uint8_t checksum = 0;
    int length = strlen(thing);
    int i;
    for (i = 0; i < length; i++) {
        checksum ^= thing[i];
    }
    return checksum;
}

/**
 * Encodes the coordinate data for a guess into the string `message`. This string must be big
 * enough to contain all of the necessary data. The format is specified in PAYLOAD_TEMPLATE_COO,
 * which is then wrapped within the message as defined by MESSAGE_TEMPLATE. The final length of this
 * message is then returned. There is no failure mode for this function as there is no checking
 * for NULL pointers.
 * @param message The character array used for storing the output. Must be long enough to store the
 *                entire string, see PROTOCOL_MAX_MESSAGE_LEN.
 * @param data The data struct that holds the data to be encoded into `message`.
 * @return The length of the string stored into `message`.
 */

int ProtocolEncodeCooMessage(char *message, const GuessData * data)
{
    char coo[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;
    sprintf(coo, PAYLOAD_TEMPLATE_COO, data->row, data->col);
    checksum = checksummer(coo);
    sprintf(message, MESSAGE_TEMPLATE, coo, checksum);
    return strlen(message);
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeHitMessage(char *message, const GuessData *data)
{
    char hit[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;
    sprintf(hit, PAYLOAD_TEMPLATE_HIT, data->row, data->col, data->hit);
    checksum = checksummer(hit);
    sprintf(message, MESSAGE_TEMPLATE, hit, checksum);
    return strlen(message);
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeChaMessage(char *message, const NegotiationData *data)
{
    char cha[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;
    sprintf(cha, PAYLOAD_TEMPLATE_CHA, data->encryptedGuess, data->hash);
    checksum = checksummer(cha);
    sprintf(message, MESSAGE_TEMPLATE, cha, checksum);
    return strlen(message);
}

/**
 * Follows from ProtocolEncodeCooMessage above.
 */
int ProtocolEncodeDetMessage(char *message, const NegotiationData *data)
{
    char det[PROTOCOL_MAX_PAYLOAD_LEN];
    uint8_t checksum;
    sprintf(det, PAYLOAD_TEMPLATE_DET, data->guess, data->encryptionKey);
    checksum = checksummer(det);
    sprintf(message, MESSAGE_TEMPLATE, det, checksum);
    return strlen(message);
}

/**
 * This function decodes a message into either the NegotiationData or GuessData structs depending
 * on what the type of message is. This function receives the message one byte at a time, where the
 * messages are in the format defined by MESSAGE_TEMPLATE, with payloads of the format defined by
 * the PAYLOAD_TEMPLATE_* macros. It returns the type of message that was decoded and also places
 * the decoded data into either the `nData` or `gData` structs depending on what the message held.
 * The onus is on the calling function to make sure the appropriate structs are available (blame the
 * lack of function overloading in C for this ugliness).
 *
 * PROTOCOL_PARSING_FAILURE is returned if there was an error of any kind (though this excludes
 * checking for NULL pointers), while
 * 
 * @param in The next character in the NMEA0183 message to be decoded.
 * @param nData A struct used for storing data if a message is decoded that stores NegotiationData.
 * @param gData A struct used for storing data if a message is decoded that stores GuessData.
 * @return A value from the UnpackageDataEnum enum.
 */
ProtocolParserStatus ProtocolDecode(char in, NegotiationData *nData, GuessData *gData)
{
    char msgcheck[4];
    int i;
    //char token[PROTOCOL_MAX_MESSAGE_LEN];
    switch (decodedata.check) {
    case WAITING:
        if (in == '$') {
            decodedata.index = 0;
            decodedata.check = RECORDING;
            return PROTOCOL_PARSING_GOOD;
        } else {
            return PROTOCOL_WAITING;
        }
        break;
    case RECORDING:
        if (in != '*') {
            decodedata.message[decodedata.index] = in;
            decodedata.index++;
            return PROTOCOL_PARSING_GOOD;
        } else {
            decodedata.check = FIRST_CHECKSUM_HALF;
            return PROTOCOL_PARSING_GOOD;
            break;
        }
    case FIRST_CHECKSUM_HALF:
        if (ASCIItoHEX(in) != -1) {
            decodedata.checksum = ASCIItoHEX(in) << 4;
            decodedata.check = SECOND_CHECKSUM_HALF;
            return PROTOCOL_PARSING_GOOD;
        } else {
            decodedata.check = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;
    case SECOND_CHECKSUM_HALF:
        decodedata.checksum += ASCIItoHEX(in);
        if (ASCIItoHEX(in) != -1 && decodedata.checksum == checksummer(decodedata.message)) {
            decodedata.message[decodedata.index] = '\0';
            decodedata.check = NEWLINE;
            return PROTOCOL_PARSING_GOOD;
        } else {
            decodedata.check = WAITING;
            return PROTOCOL_PARSING_FAILURE;
        }
        break;
    case NEWLINE:
        i = 0;
        const char comma[2] = ",";
        int val1, val2, val3;
        while (i < 3) {
            msgcheck[i] = decodedata.message[i];
            i++;
        }
        msgcheck[3] = '\0';
        if ((in == '\n'|| in == '\r') && (strcmp(msgcheck, "DET") == 0 || strcmp(msgcheck, "CHA") == 0 || strcmp(msgcheck, "HIT") == 0 || strcmp(msgcheck, "COO") == 0)) {
            if (strcmp(msgcheck, "DET") == 0) {
                strtok(decodedata.message, comma);
                val1 = atoi(strtok(NULL, comma));
                val2 = atoi(strtok(NULL, comma));
                nData->guess = val1;
                nData->encryptionKey = val2;
                decodedata.check = WAITING;
                memset(decodedata.message,0,sizeof(decodedata.message));
                return PROTOCOL_PARSED_DET_MESSAGE;
            } else if (strcmp(msgcheck, "CHA") == 0) {
                strtok(decodedata.message, comma);
                val1 = atoi(strtok(NULL, comma));
                val2 = atoi(strtok(NULL, comma));
                nData->encryptedGuess = val1;
                nData->hash = val2;
                decodedata.check = WAITING;
                memset(decodedata.message,0,sizeof(decodedata.message));
                return PROTOCOL_PARSED_CHA_MESSAGE;
            } else if (strcmp(msgcheck, "HIT") == 0) {
                strtok(decodedata.message, comma);
                val1 = atoi(strtok(NULL, comma));
                val2 = atoi(strtok(NULL, comma));
                val3 = atoi(strtok(NULL, comma));
                gData->row = val1;
                gData->col = val2;
                gData->hit = val3;
                decodedata.check = WAITING;
                memset(decodedata.message,0,sizeof(decodedata.message));
                return PROTOCOL_PARSED_HIT_MESSAGE;
            } else if (strcmp(msgcheck, "COO") == 0) {
                strtok(decodedata.message, comma);
                val1 = atoi(strtok(NULL, comma));
                val2 = atoi(strtok(NULL, comma));
                gData->row = val1;
                gData->col = val2;
                decodedata.check = WAITING;
                memset(decodedata.message,0,sizeof(decodedata.message));
                return PROTOCOL_PARSED_COO_MESSAGE;
            }
        } else {
            decodedata.check = WAITING;
            return PROTOCOL_PARSING_FAILURE;
            break;
        }
    }
    return PROTOCOL_PARSING_FAILURE;
}

/**
 * This function generates all of the data necessary for the negotiation process used to determine
 * the player that goes first. It relies on the pseudo-random functionality built into the standard
 * library. The output is stored in the passed NegotiationData struct. The negotiation data is
 * generated by creating two random 16-bit numbers, one for the actual guess and another for an
 * encryptionKey used for encrypting the data. The 'encryptedGuess' is generated with an
 * XOR(guess, encryptionKey). The hash is simply an 8-bit value that is the XOR() of all of the
 * bytes making up both the guess and the encryptionKey. There is no checking for NULL pointers
 * within this function.
 * @param data The struct used for both input and output of negotiation data.
 */
void ProtocolGenerateNegotiationData(NegotiationData *data)
{
    data->guess = rand() & 0xFFFF;
    data->encryptionKey = rand() & 0xFFFF;
    data->hash = 0;
    data->encryptedGuess = data->guess ^ data->encryptionKey;
    data->hash ^= (data->guess & 0x00FF);
    data->hash ^= ((data->guess & 0xFF00) >> 8);
    data->hash ^= (data->encryptionKey & 0x00FF);
    data->hash ^= ((data->encryptionKey & 0xFF00) >> 8);
}

/**
 * Validates that the negotiation data within 'data' is correct according to the algorithm given in
 * GenerateNegotitateData(). Used for verifying another agent's supplied negotiation data. There is
 * no checking for NULL pointers within this function. Returns TRUE if the NegotiationData struct
 * is valid or FALSE on failure.
 * @param data A filled NegotiationData struct that will be validated.
 * @return TRUE if the NegotiationData struct is consistent and FALSE otherwise.
 */
uint8_t ProtocolValidateNegotiationData(const NegotiationData * data)
{
    int truth = 0;
    uint8_t temp = 0;

    if (data->encryptedGuess == (data->guess ^ data->encryptionKey)) {
        truth++;
    }
    temp ^= (data->guess & 0x00FF);
    temp ^= ((data->guess & 0xFF00) >> 8);
    temp ^= (data->encryptionKey & 0x00FF);
    temp ^= ((data->encryptionKey & 0xFF00) >> 8);
    if (temp == data->hash) {
        truth++;
    }
    if (truth == 2) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/**
 * This function returns a TurnOrder enum type representing which agent has won precedence for going
 * first. The value returned relates to the agent whose data is in the 'myData' variable. The turn
 * ordering algorithm relies on the XOR() of the 'encryptionKey' used by both agents. The least-
 * significant bit of XOR(myData.encryptionKey, oppData.encryptionKey) is checked so that if it's a
 * 1 the player with the largest 'guess' goes first otherwise if it's a 0, the agent with the
 * smallest 'guess' goes first. The return value of TURN_ORDER_START indicates that 'myData' won,
 * TURN_ORDER_DEFER indicates that 'oppData' won, otherwise a tie is indicated with TURN_ORDER_TIE.
 * There is no checking for NULL pointers within this function.
 * @param myData The negotiation data representing the current agent.
 * @param oppData The negotiation data representing the opposing agent.
 * @return A value from the TurnOrdering enum representing which agent should go first.
 */
TurnOrder ProtocolGetTurnOrder(const NegotiationData *myData, const NegotiationData * oppData)
{
    int lsb;
    lsb = (myData->encryptionKey ^ oppData->encryptionKey) & 1;
    if (lsb) {
        if (myData->guess > oppData->guess) {
            return TURN_ORDER_START;
        } else if (myData->guess == oppData->guess) {
            return TURN_ORDER_TIE;
        } else {
            return TURN_ORDER_DEFER;
        }
    } else {
        if (myData->guess < oppData->guess) {
            return TURN_ORDER_START;
        } else if (myData->guess == oppData->guess) {
            return TURN_ORDER_TIE;
        } else {
            return TURN_ORDER_DEFER;
        }
    }
}