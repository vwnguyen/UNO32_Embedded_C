#include <stdint.h>
#include <GenericTypeDefs.h>
#include "Protocol.h"
#include "Field.h"
/**
 * Define the dimensions of the game field. They can be overridden by compile-time specifications.
 * All references to the dimensions of the field should use these constants instead of hard-coding
 * a numeric value so that the field dimensions can be changed with minimal coding changes.
 */

/**
 * FieldInit() will fill the passed field array with the data specified in positionData. Also the
 * lives for each boat are filled according to the `BoatLives` enum.
 * @param f The field to initialize.
 * @param p The data to initialize the entire field to, should be a member of enum
 *                     FieldPosition.
 */
void FieldInit(Field *f, FieldPosition p) {
    f->hugeBoatLives = FIELD_BOAT_LIVES_HUGE;
    f->mediumBoatLives = FIELD_BOAT_LIVES_MEDIUM;
    f->largeBoatLives = FIELD_BOAT_LIVES_LARGE;
    f->smallBoatLives = FIELD_BOAT_LIVES_SMALL;
    int i = 0;
    int j = 0;
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            f->field[i][j] = p;
        }
    }
}

/**
 * Retrieves the value at the specified field position.
 * @param f The Field being referenced
 * @param row The row-component of the location to retrieve
 * @param col The column-component of the location to retrieve
 * @return
 */
FieldPosition FieldAt(const Field *f, uint8_t row, uint8_t col) {
    //return f->field[row][col];
    // lifecheck checks the old value
    uint32_t lifeCheck = f->field[row][col];
    if (lifeCheck == FIELD_POSITION_EMPTY) {
        return FIELD_POSITION_EMPTY;
    }
    if (lifeCheck == FIELD_POSITION_HUGE_BOAT) {
        return FIELD_POSITION_HUGE_BOAT;
    }
    if ( lifeCheck == FIELD_POSITION_HIT){
        return FIELD_POSITION_HIT;
    }
    if (lifeCheck == FIELD_POSITION_MEDIUM_BOAT) {
        return FIELD_POSITION_MEDIUM_BOAT;
    }
    if (lifeCheck == FIELD_POSITION_SMALL_BOAT) {
        return FIELD_POSITION_SMALL_BOAT;
    }
    if ( lifeCheck == FIELD_POSITION_UNKNOWN){
        return FIELD_POSITION_UNKNOWN;
    }
    if (lifeCheck == FIELD_POSITION_LARGE_BOAT) {
        return FIELD_POSITION_LARGE_BOAT;
    }
    // never makes this return
    return lifeCheck;
}

/**
 * This function provides an interface for setting individual locations within a Field struct. This
 * is useful when FieldAddBoat() doesn't do exactly what you need. For example, if you'd like to use
 * FIELD_POSITION_CURSOR, this is the function to use.
 * 
 * @param f The Field to modify.
 * @param row The row-component of the location to modify
 * @param col The column-component of the location to modify
 * @param p The new value of the field location
 * @return The old value at that field location
 */
FieldPosition FieldSetLocation(Field *f, uint8_t row, uint8_t col, FieldPosition p) {
    FieldPosition oldValue = f->field[row][col];
    f->field[row][col] = p;
    return oldValue;
}

/**
 * FieldAddBoat() places a single ship on the player's field based on arguments 2-5. Arguments 2, 3
 * represent the x, y coordinates of the pivot point of the ship.  Argument 4 represents the
 * direction of the ship, and argument 5 is the length of the ship being placed. All spaces that
 * the boat would occupy are checked to be clear before the field is modified so that if the boat
 * can fit in the desired position, the field is modified as SUCCESS is returned. Otherwise the
 * field is unmodified and STANDARD_ERROR is returned. There is no hard-coded limit to how many
 * times a boat can be added to a field within this function.
 *
 * So this is valid test code:
 * {
 *   Field myField;
 *   FieldInit(&myField,FIELD_POSITION_EMPTY);
 *   FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
 *   FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
 *   FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
 * }
 *
 * should result in a field like:
 *  _ _ _ _ _ _ _ _
 * [ 3 3 3       3 ]
 * [ 4 4 4 4     3 ]
 * [             3 ]
 *  . . . . . . . .
 *
 * @param f The field to grab data from.
 * @param row The row that the boat will start from, valid range is from 0 and to FIELD_ROWS - 1.
 * @param col The column that the boat will start from, valid range is from 0 and to FIELD_COLS - 1.
 * @param dir The direction that the boat will face once places, from the BoatDirection enum.
 * @param boatType The type of boat to place. Relies on the FIELD_POSITION_*_BOAT values from the
 * FieldPosition enum.
 * @return TRUE for success, FALSE for failure
 */

uint8_t FieldAddBoat(Field *f, uint8_t row, uint8_t col, BoatDirection dir, BoatType type) {
    int i = row;
    int j = col;
    int z = 0;
    // NORTH CASE
    if (dir == FIELD_BOAT_DIRECTION_NORTH) { // check the direction
        if (type == FIELD_BOAT_SMALL) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 3; z++) {
                if (i < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i--;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 3; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_SMALL_BOAT);
                i--;
            }
        }
        if (type == FIELD_BOAT_MEDIUM) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 4; z++) {
                if (i < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i--;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 4; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_MEDIUM_BOAT);
                i--;
            }
        }
        if (type == FIELD_BOAT_LARGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 5; z++) {
                if (i < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i--;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 5; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_LARGE_BOAT);
                i--;
            }
        }
        if (type == FIELD_BOAT_HUGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 6; z++) {
                if (i < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i--;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 6; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_HUGE_BOAT);
                i--;
            }
        }
    }
    // SOUTH CASE
    if (dir == FIELD_BOAT_DIRECTION_SOUTH) { // check the direction
        if (type == FIELD_BOAT_SMALL) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 3; z++) {
                if (i > FIELD_ROWS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i++;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 3; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_SMALL_BOAT);
                i++;
            }
        }
        if (type == FIELD_BOAT_MEDIUM) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 4; z++) {
                if (i > FIELD_ROWS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i++;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 4; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_MEDIUM_BOAT);
                i++;
            }
        }
        if (type == FIELD_BOAT_LARGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 5; z++) {
                if (i > FIELD_ROWS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i++;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 5; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_LARGE_BOAT);
                i++;
            }
        }
        if (type == FIELD_BOAT_HUGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 6; z++) {
                if (i > FIELD_ROWS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                i++;
            }
            i = row;
            // if we have an empty spot, place it in there
            for (z = 0; z < 6; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_HUGE_BOAT);
                i++;
            }
        }
    }
    // WEST CASE
    if (dir == FIELD_BOAT_DIRECTION_WEST) { // check the direction
        if (type == FIELD_BOAT_SMALL) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 3; z++) {
                if (j < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j--;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 3; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_SMALL_BOAT);
                j--;
            }
        }
        if (type == FIELD_BOAT_MEDIUM) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 4; z++) {
                if (j < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j--;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 4; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_MEDIUM_BOAT);
                j--;
            }
        }
        if (type == FIELD_BOAT_LARGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 5; z++) {
                if (j < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j--;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 5; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_LARGE_BOAT);
                j--;
            }
        }
        if (type == FIELD_BOAT_HUGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 6; z++) {
                if (j < 0 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j--;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 6; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_HUGE_BOAT);
                j--;
            }
        }
    }
    // EAST CASE
    if (dir == FIELD_BOAT_DIRECTION_EAST) { // check the direction
        if (type == FIELD_BOAT_SMALL) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 3; z++) {
                if (j > FIELD_COLS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j++;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 3; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_SMALL_BOAT);
                j++;
            }
        }
        if (type == FIELD_BOAT_MEDIUM) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 4; z++) {
                if (j > FIELD_COLS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j++;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 4; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_MEDIUM_BOAT);
                j++;
            }
        }
        if (type == FIELD_BOAT_LARGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 5; z++) {
                if (j > FIELD_COLS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j++;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 5; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_LARGE_BOAT);
                j++;
            }
        }
        if (type == FIELD_BOAT_HUGE) { // check the size
            // check if it is empty or not OR if we run off of the board
            for (z = 0; z < 6; z++) {
                if (j > FIELD_COLS - 1 || FieldAt(f, i, j) != FIELD_POSITION_EMPTY) {
                    return FALSE; // return false and abort this operation
                }
                j++;
            }
            j = col;
            // if we have an empty spot, place it in there
            for (z = 0; z < 6; z++) {
                FieldSetLocation(f, i, j, FIELD_POSITION_HUGE_BOAT);
                j++;
            }
        }
    }
    return TRUE;
}

/**
 * This function registers an attack at the gData coordinates on the provided field. This means that
 * 'f' is updated with a FIELD_POSITION_HIT or FIELD_POSITION_MISS depending on what was at the
 * coordinates indicated in 'gData'. 'gData' is also updated with the proper HitStatus value
 * depending on what happened AND the value of that field position BEFORE it was attacked. Finally
 * this function also reduces the lives for any boat that was hit from this attack.
 * @param f The field to check against and update.
 * @param gData The coordinates that were guessed. The HIT result is stored in gData->hit as an
 *               output.
 * @return The data that was stored at the field position indicated by gData before this attack.
 */
FieldPosition FieldRegisterEnemyAttack(Field *f, GuessData *gData) {
    uint32_t guessCol = gData->col;
    uint32_t guessRow = gData->row;
    // grab the previous data
    uint32_t previousLocationInfo = FieldAt(f, guessRow, guessCol);
    if (f->field[guessRow][guessCol] == FIELD_POSITION_EMPTY) {
        gData->hit = HIT_MISS;
        //return previousLocationInfo;
    }
    if (previousLocationInfo == FIELD_POSITION_HUGE_BOAT) {
        f->hugeBoatLives--;
        if (f->hugeBoatLives > 0) {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_HIT;
        } else {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_SUNK_HUGE_BOAT;
        }
        //return previousLocationInfo;
    }
    if (previousLocationInfo == FIELD_POSITION_LARGE_BOAT) {
        f->largeBoatLives--;
        if (f->largeBoatLives > 0) {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_HIT;
        } else {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_SUNK_LARGE_BOAT;
        }
        //return previousLocationInfo;
    }
    if (previousLocationInfo == FIELD_POSITION_MEDIUM_BOAT) {
        f->mediumBoatLives--;
        if (f->mediumBoatLives > 0) {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_HIT;
        } else {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_SUNK_MEDIUM_BOAT;
        }
        //return previousLocationInfo;
    }
    if (previousLocationInfo == FIELD_POSITION_SMALL_BOAT) {
        f->smallBoatLives--;
        if (f->smallBoatLives > 0) {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_HIT;
        } else {
            FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
            gData->hit = HIT_SUNK_SMALL_BOAT;
        }
        //return previousLocationInfo;
    }
    return previousLocationInfo;
}

/**
 * This function updates the FieldState representing the opponent's game board with whether the
 * guess indicated within gData was a hit or not. If it was a hit, then the field is updated with a
 * FIELD_POSITION_HIT at that position. If it was a miss, display a FIELD_POSITION_EMPTY instead, as
 * it is now known that there was no boat there. The FieldState struct also contains data on how
 * many lives each ship has. Each hit only reports if it was a hit on any boat or if a specific boat
 * was sunk, this function also clears a boats lives if it detects that the hit was a
 * HIT_SUNK_*_BOAT. 
 * @param f The field to grab data from.
 * @param gData The coordinates that were guessed along with their HitStatus.
 * @return The previous value of that coordinate position in the field before the hit/miss was
 * registered.
 */
// THIS IS FOR THE ENEMY FIELD
FieldPosition FieldUpdateKnowledge(Field *f, const GuessData *gData) {
    uint32_t hitOrNot = gData->hit;
    uint32_t guessRow = gData->row;
    uint32_t guessCol = gData->col;
    // if we have hit OR sunk a boat, update field location
    if (hitOrNot == HIT_HIT || hitOrNot == HIT_SUNK_HUGE_BOAT ||
            hitOrNot == HIT_SUNK_MEDIUM_BOAT || hitOrNot == HIT_SUNK_LARGE_BOAT
            || hitOrNot == HIT_SUNK_SMALL_BOAT) {
        FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_HIT);
        if (hitOrNot == HIT_SUNK_HUGE_BOAT) {
            f->hugeBoatLives = 0;
        }
        if (hitOrNot == HIT_SUNK_LARGE_BOAT) {
            f->largeBoatLives = 0;
        }
        if (hitOrNot == HIT_SUNK_MEDIUM_BOAT) {
            f->mediumBoatLives = 0;
        }
        if (hitOrNot == HIT_SUNK_SMALL_BOAT) {
            f->smallBoatLives = 0;
        }
    }
    if ( hitOrNot == HIT_MISS){
        FieldSetLocation(f, guessRow, guessCol, FIELD_POSITION_EMPTY);
    }
    return FieldAt(f, guessRow, guessCol);
}

/**
 * This function returns the alive states of all 4 boats as a 4-bit bitfield (stored as a uint8).
 * The boats are ordered from smallest to largest starting at the least-significant bit. So that:
 * 0b00001010 indicates that the small boat and large boat are sunk, while the medium and huge boat
 * are still alive. See the BoatStatus enum for the bit arrangement.
 * @param f The field to grab data from.
 * @return A 4-bit value with each bit corresponding to whether each ship is alive or not.
 */
uint8_t FieldGetBoatStates(const Field *f) {
    uint8_t result = 0;
    if (f->largeBoatLives > 0) {
        result = result | FIELD_BOAT_STATUS_LARGE;
    }
    if (f->mediumBoatLives > 0) {
        result = result | FIELD_BOAT_STATUS_MEDIUM;
    }
    if (f->smallBoatLives > 0) {
        result = result | FIELD_BOAT_STATUS_SMALL;
    }
    if (f->hugeBoatLives > 0) {
        result = result | FIELD_BOAT_STATUS_HUGE;
    }
    return result;
}
