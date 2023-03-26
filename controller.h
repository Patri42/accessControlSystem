#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "SystemState.h"
#include <stdbool.h>

// Function declarations
void waitForInput();
void printLampStatus(bool isGreen);
void remoteOpenDoor(struct SystemState* state);
int getChoice();
void updateCardAccess(struct SystemState* state, int cardIndex, int choice);
void addNewCard(struct SystemState* state, char* cardNum, int choice);
char* getCardNumber();
void addRemoveAccess(struct SystemState* state);
void fakeCardScan(struct SystemState* state);
void listAllCards(struct SystemState* state);
int loadCardData(struct SystemState* state);
int getUserChoice();
void processUserChoice(int choice, struct SystemState* state);

#endif // CONTROLLER_H