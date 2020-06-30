// Heap size 2048 required!

// **** Include libraries here ****
// Standard libraries
#include <stdio.h>

//CMPE13 Support Library
#include "BOARD.h"

// Microchip libraries
#include <xc.h>
#include <math.h>
#include <plib.h>

// User libraries
#include "BinaryTree.h"

// **** Set any macros or preprocessor directives here ****


// **** Define any module-level, global, or external variables here ****


// **** Declare any function prototypes here ****

int main() {
    BOARD_Init();

    /******************************************************************************
     * Your code goes in between this comment and the following one with asterisks.
     *****************************************************************************/
    //printf("\nWelcome to the BinaryTree testing program, please delete this line and replace with your own");
    Node* N = newNode("A");
    printf("%c \n", N->data);
    int howManyElements = pow(2,3);
    const char *input = "#EISH54V#3UF####2ARL#####WP##J#1TNDB6#X##KC##Y##MGZ7#Q##O#8##90";
    howManyElements = howManyElements - 1;
    printf("%d\n", howManyElements);
    printf("------------------------------------------------------\n");
    Node* ROOT = TreeCreate(3, "ABDECFG");
    PrintTree(ROOT, 0);
    printf("------------------------------------------------------\n");
    printf("------------------------------------------------------\n");
    //Node* ROOT1 = TreeCreate(3," EIATNM");
    //Node* ROOT1 = TreeCreate(4," EISUARWTNDKMGO");
    //Node* ROOT1 = TreeCreate(5," EISHVUF ARL WPJTNDBXKCYMGZQO  ");
    //const char* A = " EISH54V 3UF    2ARL     WP  J 1TNDB6 X  KC  Y  MGZ7 Q  O 8  90";
    Node* ROOT1 = TreeCreate(6," EISH54V 3UF    2ARL     WP  J 1TNDB6 X  KC  Y  MGZ7 Q  O 8  90");
    //Node* ROOT1 = TreeCreate(6,input);
    PrintTree(ROOT1,0);
    printf("------------------------------------------------------\n");
    
    
    Node* right = GetRightChild(ROOT);
    Node* left = GetLeftChild(ROOT);
    printf("Right Child of ROOT: %c\n", right->data);
    printf("Left Child of ROOT: %c\n", left->data);

    Node* right1 = GetRightChild(right);
    Node* left1 = GetLeftChild(left);
    printf("Right Child of C: %c\n", right1->data);
    printf("Left Child of B: %c\n", left1->data);
    
    Node* right2 = GetRightChild(right1);
    Node* left2 = GetLeftChild(left1);
    if ( right2 == NULL ){
        printf("right2 == NULL!\n");
    }
    if ( left2 == NULL ){
        printf("left2 == NULL!\n");
    }
    
//    printf("Right Child of G: %c\n", right2->data);
//    printf("Left Child of D: %c\n", left2->data);
    
    /******************************************************************************
     * Your code goes in between this comment and the preceding one with asterisks
     *****************************************************************************/

    while (1);
}

