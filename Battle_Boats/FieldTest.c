// **** Include libraries here ****
// Standard libraries

//CMPE13 Support Library
#include "BOARD.h"
#include "stdio.h"
// Microchip libraries
#include <xc.h>
#include <plib.h>
// User libraries
#include "Field.h"
#include "FieldOled.h"


// **** Set any macros or preprocessor directives here ****

// **** Declare any data types here ****

// **** Define any module-level, global, or external variables here ****


// **** Declare any function prototypes here ****

int main()
{
    BOARD_Init();
    Field testField;
    FieldInit(&testField, FIELD_POSITION_EMPTY);
    int hugeBoatLives = testField.hugeBoatLives;
    printf("Boat Lives Test HUGE: %d\n", hugeBoatLives);
    printf("Boat Lives Test LARGE: %d\n", testField.largeBoatLives);
    printf("Boat Lives Test MEDIUM: %d\n", testField.mediumBoatLives);
    printf("Boat Lives Test SMALL: %d\n", testField.smallBoatLives);
    int i = 0;
    int j = 0;
    //    for (i = 0; i < FIELD_ROWS; i++) {
    //        for (j = 0; j < FIELD_COLS; j++) {
    //            if (FieldAt(&testField, i, j) == FIELD_POSITION_EMPTY) {
    //                printf("Field Position at [%d] [%d] is EMPTY\n", i, j);
    //            }
    //        }
    //    }
    i = 0;
    j = 0;

    printf("NORTH TEST\n");
    uint8_t goodBoat = FieldAddBoat(&testField, i, j, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TEST FOR OFF THE BOARD: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 2, j, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING STANDARD CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 2, j, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING NOT EMPTY CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 3, 1, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_MEDIUM);
    goodBoat = FieldAddBoat(&testField, 4, 2, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_LARGE);
    goodBoat = FieldAddBoat(&testField, 5, 3, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_HUGE);
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", FieldAt(&testField, i, j));
        }
        printf("\n");
    }
    FieldInit(&testField, FIELD_POSITION_EMPTY);
    printf("WEST TEST\n");
    goodBoat = FieldAddBoat(&testField, 0, 9, FIELD_BOAT_DIRECTION_WEST, FIELD_BOAT_SMALL);
    goodBoat = FieldAddBoat(&testField, 0, 0, FIELD_BOAT_DIRECTION_WEST, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING RUN OFF CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 1, 8, FIELD_BOAT_DIRECTION_WEST, FIELD_BOAT_MEDIUM);
    goodBoat = FieldAddBoat(&testField, 2, 7, FIELD_BOAT_DIRECTION_WEST, FIELD_BOAT_LARGE);
    goodBoat = FieldAddBoat(&testField, 3, 6, FIELD_BOAT_DIRECTION_WEST, FIELD_BOAT_HUGE);
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", FieldAt(&testField, i, j));
        }
        printf("\n");
    }
    printf("SOUTH TEST\n");
    FieldInit(&testField, FIELD_POSITION_EMPTY);
    goodBoat = FieldAddBoat(&testField, 4, 0, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TEST FOR OFF THE BOARD: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 0, 0, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING STANDARD CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 0, 0, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING NOT EMPTY CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 0, 1, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_MEDIUM);
    goodBoat = FieldAddBoat(&testField, 0, 2, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_LARGE);
    goodBoat = FieldAddBoat(&testField, 0, 3, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_HUGE);
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", FieldAt(&testField, i, j));
        }
        printf("\n");
    }
    printf("EAST TEST\n");
    FieldInit(&testField, FIELD_POSITION_EMPTY);
    goodBoat = FieldAddBoat(&testField, 0, 9, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TEST FOR OFF THE BOARD: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING STANDARD CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 0, 0, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING NOT EMPTY CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&testField, 1, 1, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
    goodBoat = FieldAddBoat(&testField, 2, 2, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_LARGE);
    goodBoat = FieldAddBoat(&testField, 3, 3, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", testField.field[i][j]);
        }
        printf("\n");
    }
    //    printf("THEIR DEFAULT TEST\n");
    //    Field myField;
    //    FieldInit(&myField, FIELD_POSITION_EMPTY);
    //    FieldAddBoat(&myField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
    //    FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
    //    FieldAddBoat(&myField, 1, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);
    //    FieldAddBoat(&myField, 0, 6, FIELD_BOAT_DIRECTION_SOUTH, FIELD_BOAT_SMALL);
    //    for (i = 0; i < FIELD_ROWS; i++) {
    //        for (j = 0; j < FIELD_COLS; j++) {
    //            printf("%d   ", FieldAt(&myField, i, j));
    //        }
    //        printf("\n");
    //    }

    printf("FIELDAT, FIELDREGATTACK TESTS\n");
    GuessData wrongGuess;
    wrongGuess.row = 5;
    wrongGuess.col = 0;
    GuessData correctGuess;
    correctGuess.row = 0;
    correctGuess.col = 0;
    GuessData correctGuess2;
    correctGuess2.row = 0;
    correctGuess2.col = 1;
    GuessData correctGuess3;
    correctGuess3.row = 0;
    correctGuess3.col = 2;
    GuessData correctGuess4;
    correctGuess4.row = 1;
    correctGuess4.col = 1;

    printf("Default boat lives? %d\n", testField.smallBoatLives);
    uint32_t location1 = FieldRegisterEnemyAttack(&testField, &wrongGuess);
    printf("Did the lives update after WRONG GUESS? %d\n", testField.smallBoatLives);
    uint32_t location2 = FieldRegisterEnemyAttack(&testField, &correctGuess);
    printf("location at the wrong guess?: %d\n", location1);
    printf("location at the right guess? %d\n", location2);
    printf("Did the lives update? should be 2: %d\n", testField.smallBoatLives);
    printf("Did the field update after the hits?\n");

    FieldRegisterEnemyAttack(&testField, &correctGuess2);
    FieldRegisterEnemyAttack(&testField, &correctGuess3);
    FieldRegisterEnemyAttack(&testField, &correctGuess4);

    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", FieldAt(&testField, i, j));
        }
        printf("\n");
    }
    
    
    i = 0;
    j = 0;
    printf("ALL BOATS ALIVE? %d\n", FieldGetBoatStates(&testField));
    Field enemytestField;
    FieldInit(&enemytestField, FIELD_POSITION_UNKNOWN);
    // initially unknown field
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", FieldAt(&enemytestField, i, j));
        }
        printf("\n");
    }
    goodBoat = FieldAddBoat(&enemytestField, 0, 9, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TEST FOR OFF THE BOARD: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&enemytestField, 0, 0, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING STANDARD CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&enemytestField, 0, 0, FIELD_BOAT_DIRECTION_NORTH, FIELD_BOAT_SMALL);
    printf("goodBoat placement? TESTING NOT EMPTY CASE: %d\n", goodBoat);
    goodBoat = FieldAddBoat(&enemytestField, 1, 1, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_MEDIUM);
    goodBoat = FieldAddBoat(&enemytestField, 2, 2, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_LARGE);
    goodBoat = FieldAddBoat(&enemytestField, 3, 3, FIELD_BOAT_DIRECTION_EAST, FIELD_BOAT_HUGE);

    GuessData testGuess;
    testGuess.row = 0;
    testGuess.col = 0;
    testGuess.hit = HIT_HIT;
    GuessData testGuess2;
    testGuess2.row = 0;
    testGuess2.col = 1;
    testGuess2.hit = HIT_HIT;
    GuessData testGuess3;
    testGuess3.row = 0;
    testGuess3.col = 2;
    testGuess3.hit = HIT_SUNK_SMALL_BOAT;
    GuessData testGuess4;
    testGuess4.row = 0;
    testGuess4.col = 3;
    testGuess4.hit = HIT_MISS;

    FieldUpdateKnowledge(&enemytestField, &testGuess);
    FieldUpdateKnowledge(&enemytestField, &testGuess2);
    FieldUpdateKnowledge(&enemytestField, &testGuess3);
    FieldUpdateKnowledge(&enemytestField, &testGuess4);
    // print the updated enemy field
    for (i = 0; i < FIELD_ROWS; i++) {
        for (j = 0; j < FIELD_COLS; j++) {
            printf("%d   ", FieldAt(&enemytestField, i, j));
        }
        printf("\n");
    }

    //FieldUpdateKnowledge(&enemytestField,)

    while (1);
}