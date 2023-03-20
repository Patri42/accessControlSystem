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

void addRemoveAccess(struct SystemState* state){
    char* cardNum = malloc (CARD_NUM_LEN*sizeof(char));
    printf("Enter Cardnumber: ");
    fgets(cardNum, CARD_NUM_LEN, stdin);
    cardNum[strcspn(cardNum, "\n")] = '\0';
}


int main(){
    
    struct SystemState state;
    remoteOpenDoor(&state);

    // Testing
    struct Card card1 = {"12345", 1, time(0)};
    struct Card card2 = {"67890", 0, time(0)};

    state.numCards=2;
    state.cards = (struct Card*) malloc (state.numCards* sizeof(struct Card));
    state.cards[0]= card1;
    state.cards[1]= card2;

    listAllCards(&state);
    free (state.cards);
    return 0;
}