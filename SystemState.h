#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#include <time.h> // Here because of Card struct


// Define a struct for a card
struct Card {
    char* number;
    int hasAccess;
    time_t addedTime;
};

// Define a struct for a card node
struct CardNode {
    struct Card cardInfo;
    struct CardNode* next;
};

// Define a struct for the system state
struct SystemState {
    struct CardNode* cardHead; // Pointer to the head of the linked list of cards
    int totalCards;
    char* cardDataFile;
};

#endif