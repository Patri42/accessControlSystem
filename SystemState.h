#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include <time.h> // Here because of Card struct


// Define the maximum length of a card number
#define CARD_NUM_LEN 20 

// Define a struct for a card
struct Card {
    char number[CARD_NUM_LEN];
    int hasAccess;
    time_t added;
};

// Define a struct for the system state
struct SystemState {
    struct Card* cards;
    int numCards;
};

#endif