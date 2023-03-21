#include <stdio.h>
#include <stdbool.h>
#include "SystemState.h"

void printLampStatus(bool isGreen) {
    const char* color = isGreen ? ANSI_COLOR_GREEN : ANSI_COLOR_RED;
    printf("Currently lamp is: %s%s%s\n",color, isGreen ? "GREEN" : "RED", ANSI_COLOR_RESET);
}

void remoteOpenDoor(struct SystemState* state) {
    printLampStatus(1);
    // Wait for 3 seconds
    for (int i = 3; i > 0; i--) {
        printf("Door will be open for %d seconds\n", i);
        sleep(1);
    }
    printLampStatus(0);
}

void listAllCards(struct SystemState* state){

    if (state ->numCards==0){
        printf ("No cards stored.\n");
        return;
    }

    printf("All cards in system:\n");
    for (int cardIndex = 0; cardIndex < state->numCards; cardIndex++ ){
        printf("%s", state->cards[cardIndex].number);
        if (state->cards[cardIndex].hasAccess){
            printf("Access ");
        }else {
            printf("No access ");
        }
        printf("Added to system: %s", ctime(&state->cards[cardIndex].added));
    }
}

int getChoice() {
    int choice;
    char input[100];
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        return -1;
    }
    return choice;
}

void updateCardAccess(struct SystemState* state, int cardIndex, int choice) {
    switch (choice) {
        case 1:
            state->cards[cardIndex].hasAccess = 1;
            break;
        case 2:
            state->cards[cardIndex].hasAccess = 0;
            break;
        default:
            printf("Invalid choice.\n");
            break;
    }
}

void addNewCard(struct SystemState* state, char* cardNum, int choice) {
    struct Card newCard;
    strncpy(newCard.number, cardNum, CARD_NUM_LEN - 1);
    newCard.number[CARD_NUM_LEN - 1] = '\0';
    newCard.hasAccess = choice == 1 ? 1 : 0;
    newCard.added = time(NULL);

    state->numCards++;
    state->cards = realloc(state->cards, state->numCards * sizeof(struct Card));
    state->cards[state->numCards - 1] = newCard;
}

void addRemoveAccess(struct SystemState* state) {
    char* cardNum = malloc(CARD_NUM_LEN * sizeof(char));
    printf("Enter Cardnumber: ");
    fgets(cardNum, CARD_NUM_LEN, stdin);
    cardNum[strcspn(cardNum, "\n")] = '\0';

    for (int cardIndex = 0; cardIndex < state->numCards; cardIndex++) {
        if (strcmp(state->cards[cardIndex].number, cardNum) == 0) {
            printf("This card has %s!\n", state->cards[cardIndex].hasAccess ? "access" : "no access");
            printf("Press 1 for access, 2 for no access.\n");
            int choice = getChoice();
            if (choice != -1) {
                updateCardAccess(state, cardIndex, choice);
            }
            free(cardNum);
            return;
        }
    }

    printf("This card has no access!\n");
    printf("Press 1 to add access, 2 to deny access.\n");
    int choice = getChoice();
    if (choice != -1) {
        addNewCard(state, cardNum, choice);
    }
    free(cardNum);
}

void fakeCardScan(struct SystemState* state){
   
    char* cardNum = malloc(CARD_NUM_LEN* sizeof(char));
    
    printf("Please scan card to enter or X to go back to admin menu\n");
    printLampStatus(0);
    
    fgets(cardNum, CARD_NUM_LEN, stdin);
    cardNum [strcspn(cardNum,"\n")] ='\0';

    if (strcmp(cardNum, "X") == 0 || strcmp(cardNum, "x") == 0) {
        free(cardNum);
        return;
    }

    for (int cardIndex=0; cardIndex < state->numCards; cardIndex++){
        if (strcmp(state->cards[cardIndex].number, cardNum)){
            if (state->cards[cardIndex].hasAccess)
                printLampStatus(1);
            else
                printLampStatus(0);
            free(cardNum);
            return;
        }
    }
    printLampStatus(0);
    free(cardNum);
}

int main() {

    struct SystemState state;
    state.cards = NULL;
    state.numCards = 0;

    int choice;
    do {
        system("cls");
        printf("Admin menu\n");
        printf("1. Remote open door\n");
        printf("2. List all cards in system\n");
        printf("3. Add/remove access\n");
        printf("4. Exit\n");
        printf("9. FAKE TEST SCAN CARD\n");

        char input[100];
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        switch (choice) {
            case 1:
                remoteOpenDoor(&state);
                waitForInput();
                break;
            case 2:
                listAllCards(&state);
                waitForInput();
                break;
            case 3:
                addRemoveAccess(&state);
                waitForInput();
                break;
            case 4:
                printf("Goodbye!\n");
                break;
            case 9:
                fakeCardScan(&state);
                waitForInput();
                break;
            default:
                printf("Invalid choice. Please enter a number between 1 and 4 or 9.\n");
                waitForInput();
                break;
        }
    } while (choice != 4);

    free(state.cards);
    return 0;
}