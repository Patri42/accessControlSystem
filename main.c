#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Platform specific headers for sleep function
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include "SystemState.h"

// Waits for user to press Enter
void waitForInput() {
    printf("Press Enter to continue...");
    getchar();
}

void printLampStatus(bool isGreen) {
    const char* color = isGreen ? ANSI_COLOR_GREEN : ANSI_COLOR_RED;
    printf("Currently lamp is: %s%s%s\n",color, isGreen ? "GREEN" : "RED", ANSI_COLOR_RESET);
}

// Remotely opens the door and waits for 3 seconds before closing it
void remoteOpenDoor(struct SystemState* state) {
    printLampStatus(1);
    // Wait for 3 seconds
    for (int i = 3; i > 0; i--) {
        printf("Door will be open for %d seconds\n", i);
        //sleep(1);
        #ifdef _WIN32
            Sleep(1000); // Sleep duration is in milliseconds for Windows
        #else
            sleep(1);    // Sleep duration is in seconds for POSIX systems
        #endif
    }
    printLampStatus(0);
}

void listAllCards(struct SystemState* state){
    // Open the card data file for reading
    FILE* fp = fopen(state->cardDataFile, "r");
    if (fp == NULL) {
        printf("Error: could not open file %s for reading.\n", state->cardDataFile);
        return;
    }

    // If no cards found
    if (state ->numCards==0){
        printf ("No cards stored.\n");
        // Close the file
        fclose(fp);
        return;
    }

    printf("All cards in system:\n");
    // Read the card data from the file and print it to the console
    char line [100];
    while (fgets(line, sizeof(line), fp) != NULL) {
        char number[CARD_NUM_LEN];
        int hasAccess;
        time_t added;
        if (sscanf(line, "%s %d %ld", number, &hasAccess, &added) != 3) {
            printf("Error: invalid line in card data file.\n");
            fclose(fp);
            return;
        }
        printf("%s ", number);
        if (hasAccess) {
            printf("Access ");
        } else {
            printf("No access ");
        }
        printf("Added to system: %s", ctime(&added));
    }
    fclose(fp);
}

int getChoice() {
    int choice;
    char input[100];

    // Read the user input
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        // Error message if invalid integer
        printf("Invalid input. Please enter a number.\n");
        return -1; // -1 because of the choices 1 and 2
    }
    return choice;
}

void updateCardAccess(struct SystemState* state, int cardIndex, int choice) {
    switch (choice) {
        case 1:
            // Grant access to the card
            state->cards[cardIndex].hasAccess = 1;
            break;
        case 2:
            // Remove access from the card
            state->cards[cardIndex].hasAccess = 0;
            break;
        default:
            // Error message if invalid choice
            printf("Invalid choice.\n");
            break;
    }
}

void addNewCard(struct SystemState* state, char* cardNum, int choice) {
    // Creat a new card struct
    struct Card newCard;
    strncpy(newCard.number, cardNum, CARD_NUM_LEN - 1);
    newCard.number[CARD_NUM_LEN - 1] = '\0';
    newCard.hasAccess = choice == 1 ? 1 : 0;
    newCard.added = time(NULL);

    // Increase the number of cards in the system and resize the cards array
    state->numCards++;
    state->cards = realloc(state->cards, state->numCards * sizeof(struct Card));

    // Add the new card to the end of the cards array
    state->cards[state->numCards - 1] = newCard;
}

char* getCardNumber() {
    // Allocate memory for the card number
    char* cardNum = malloc(CARD_NUM_LEN * sizeof(char));
    fgets(cardNum, CARD_NUM_LEN, stdin);
    cardNum[strcspn(cardNum, "\n")] = '\0';
    return cardNum;
}

void addRemoveAccess(struct SystemState* state) {
    char* cardNum;
    printf("Enter Cardnumber: ");
    cardNum = getCardNumber();

    // Check if the card already exists in the system
    for (int cardIndex = 0; cardIndex < state->numCards; cardIndex++) {
        if (strcmp(state->cards[cardIndex].number, cardNum) == 0) {
            // If the card exists, display its current access status
            printf("This card has %s!\n", state->cards[cardIndex].hasAccess ? "access" : "no access");

            printf("Press 1 for access, 2 for no access.\n");
            int choice = getChoice();
            if (choice != -1) { // -1 because of the choices 1 and 2
                updateCardAccess(state, cardIndex, choice);
            }
            // Free memory when done using it
            free(cardNum); 
            return;
        }
    }

    // If the card is not in the system, ask the user to add it
    printf("This card has no access!\n");
    printf("Press 1 to add access, 2 to deny access.\n");
    int choice = getChoice();
    if (choice != -1) {
        addNewCard(state, cardNum, choice);
    }
    free(cardNum);
}

// Simulates a card scan and checks if the scanned card has access
void fakeCardScan(struct SystemState* state){
   
    char* cardNum;
    
    printf("Please scan card to enter or X to go back to admin menu\n");
    printLampStatus(0);
    
    cardNum = getCardNumber();

    // Check if user wants to go back to admin menu
    if (strcmp(cardNum, "X") == 0 || strcmp(cardNum, "x") == 0) {
        free(cardNum);
        return;
    }

    // Search for card in system
    for (int cardIndex=0; cardIndex < state->numCards; cardIndex++){
        if (strcmp(state->cards[cardIndex].number, cardNum)==0){
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

    // Initialize the system state
    struct SystemState state;
    state.cards = NULL;
    state.numCards = 0;

    int choice;

    // Main loop of the program
    do {
        system("cls");
        printf("Admin menu\n");
        printf("1. Remote open door\n");
        printf("2. List all cards in system\n");
        printf("3. Add/remove access\n");
        printf("4. Exit\n");
        printf("9. FAKE TEST SCAN CARD\n");

        // Get user input
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
    // Loop until the user chooses to exit
    } while (choice != 4);

    free(state.cards);
    return 0;
}