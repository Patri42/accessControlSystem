#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "controller.h"
#include "SystemState.h"


int main() {

    // Initialize the system state
    struct SystemState state;
    state.cards = NULL;
    state.numCards = 0;
    state.cardDataFile = "cards.txt";

    // Load the card data from the file
    loadCardData(&state);

    // Main loop of the program
    int choice;
    do {
        system("cls"); 
        choice = getUserChoice();
        processUserChoice(choice, &state);
    } while (choice != 4);

    // Free memory and exit the program
    free(state.cards);
    return 0;
}