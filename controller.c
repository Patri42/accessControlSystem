#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "SystemState.h"

// Platform specific headers for sleep function
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

// Function prototypes
void waitForInput();
void printLampStatus(bool isGreen);
void remoteOpenDoor(struct SystemState* state);
int getChoice();
void updateCardAccess(struct SystemState* state, int cardIndex, int choice);
void addNewCard(struct SystemState* state, const char* cardNum, int choice);
char *getCardNumber(FILE *file);
void addRemoveAccess(struct SystemState* state);
void fakeCardScan(struct SystemState* state);
void listAllCards(struct SystemState* state);
void addCardNode(struct SystemState* state, struct Card cardInfo);
int loadCardData(struct SystemState* state);
int getUserChoice();
void processUserChoice(int choice, struct SystemState* state);


// Open the cardInfo data file for reading
int loadCardData(struct SystemState* state) {
    FILE* file = fopen(state->cardDataFile, "r");
    if (file == NULL) {
        printf("Error: could not open file %s for reading.\n", state->cardDataFile);
        return 1;
    }

    char line[100];
    while (fgets(line, sizeof(line), file) != NULL) {
        struct Card cardInfo;
        char tempNumber[100]; // Use a temporary buffer to read the cardInfo number
        if (sscanf(line, "%s %d %lld", tempNumber, &cardInfo.hasAccess, &cardInfo.addedTime) != 3) {
            printf("Error: invalid line in cardInfo data file.\n");
            fclose(file);
            return 1;
        }
        // Allocate memory based on the length of the cardInfo number read from the file
        cardInfo.number = (char*) malloc(sizeof(char) * (strlen(tempNumber) + 1));
        strcpy(cardInfo.number, tempNumber);
        addCardNode(state, cardInfo);
    }

    fclose(file);
    return 0;
}

void freeCardList(struct SystemState* state) {
    struct CardNode* currentNode = state->cardHead;
    while (currentNode != NULL) {
        struct CardNode* temp = currentNode;
        currentNode = currentNode->next;
        free(temp->cardInfo.number);
        free(temp);
    }
    state->cardHead = NULL;
}

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

void listAllCards(struct SystemState* state) {
    if (state->totalCards == 0) {
        printf("No cards stored.\n");
        return;
    }

    printf("All cards in system:\n");
    struct CardNode* currentNode = state->cardHead;
    while (currentNode != NULL) {
        printf("%s ", currentNode->cardInfo.number);
        if (currentNode->cardInfo.hasAccess) {
            printf("Access ");
        } else {
            printf("No access ");
        }
        printf("Added to system: %s", ctime(&currentNode->cardInfo.addedTime));
        currentNode = currentNode->next;
    }
}

void addCardNode(struct SystemState* state, struct Card cardInfo) {
    struct CardNode* newNode = (struct CardNode*) malloc(sizeof(struct CardNode));
    newNode->cardInfo = cardInfo;
    newNode->next = NULL;

    if (state->cardHead == NULL) {
        state->cardHead = newNode;
    } else {
        struct CardNode* currentNode = state->cardHead;
        while (currentNode->next != NULL) {
            currentNode = currentNode->next;
        }
        currentNode->next = newNode;
    }
    state->totalCards++;
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


// Updates the access level of a cardInfo in the system state.
void updateCardAccess(struct SystemState* state, int cardIndex, int access_choice) {
    struct CardNode* currentNode = state->cardHead;
    int index = 0;

    while (currentNode != NULL) {
        if (index == cardIndex) {
            currentNode->cardInfo.hasAccess = access_choice;
            break;
        }
        index++;
        currentNode = currentNode->next;
    }
}

void addNewCard(struct SystemState* state, const char* cardNum, int access_choice) {
    // Create a new cardInfo struct
    struct Card newCard;
    newCard.number = (char*) malloc(sizeof(char) * (strlen(cardNum) + 1));
    strcpy(newCard.number, cardNum);
    newCard.hasAccess = access_choice == 1 ? 1 : 0;
    newCard.addedTime = time(NULL);

    // Open the cardInfo data file for appending
    FILE* file = fopen(state->cardDataFile, "a");
    if (file == NULL) {
        printf("Error: could not open file %s for writing.\n", state->cardDataFile);
        return;
    }

    // Write the new cardInfo data to the file
    fprintf(file, "%s %d %lld\n", newCard.number, newCard.hasAccess, newCard.addedTime);

    fclose(file);

    // Increase the number of cards in the system
    state->totalCards++;

    // Add the new cardInfo to the linked list using the addCardNode function
    addCardNode(state, newCard);
}

char* getCardNumber(FILE* file) {
    // Allocate memory for the cardInfo number
    char tempNumber[100]; // Use a temporary buffer to read the cardInfo number
    if (fscanf(file, "%s", tempNumber) == EOF) {
        return NULL; // Return NULL if the end of the file is reached
    }
    // Dynamic allocation
    char* cardNum = malloc((strlen(tempNumber) + 1) * sizeof(char));
    strcpy(cardNum, tempNumber);
    
    return cardNum;
}

void addRemoveAccess(struct SystemState* state) {
    char* cardNum;
    printf("Enter Cardnumber: ");
    cardNum = getCardNumber(stdin);
    
    // Clear input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
    // Check if the cardInfo already exists in the system
    struct CardNode* currentNode = state->cardHead;
    int cardIndex = 0;
    while (currentNode != NULL) {
        if (strcmp(currentNode->cardInfo.number, cardNum) == 0) {
            // If the cardInfo exists, display its current access status
            printf("This cardInfo has %s!\n", currentNode->cardInfo.hasAccess ? "access" : "no access");

            printf("Press 1 for access, 2 for no access.\n");
            int choice = getChoice();
            if (choice != -1) { // -1 because of the choices 1 and 2
                updateCardAccess(state, cardIndex, choice);
            }
            // Free memory when done using it
            free(cardNum);
            return;
        }
        currentNode = currentNode->next;
        cardIndex++;
    }

    // If the cardInfo is not in the system, ask the user to add it
    printf("This cardInfo has no access!\n");

    int choice;
    while (1) {
        printf("Press 1 to add access, 2 to deny access.\n");
        choice = getChoice();

        if (choice == 1 || choice == 2) {
            break;
        } else {
            printf("Invalid input. Please enter 1 or 2.\n");
        }
    }

    addNewCard(state, cardNum, choice);
    free(cardNum);
}

// Simulates a cardInfo scan and checks if the scanned cardInfo has access
void fakeCardScan(struct SystemState* state) {
    char* cardNum;

    printf("Please scan cardInfo to enter or X to go back to admin menu\n");
    printLampStatus(0);

    cardNum = getCardNumber(stdin); // Dynamic allocation

    // Check if user wants to go back to admin menu
    if (strcmp(cardNum, "X") == 0 || strcmp(cardNum, "x") == 0) {
        free(cardNum);
        return;
    }

    // Search for cardInfo in the system
    struct CardNode* currentNode = state->cardHead;
    while (currentNode != NULL) {
        if (strcmp(currentNode->cardInfo.number, cardNum) == 0) {
            if (currentNode->cardInfo.hasAccess) {
                printLampStatus(1);
            } else {
                printLampStatus(0);
            }
            free(cardNum);
            getchar();
            return;
        }
        currentNode = currentNode->next;
    }
    printLampStatus(0);
    free(cardNum);
    getchar();
}

// Display menu and get user choice
int getUserChoice() {
    printf("Admin menu\n");
    printf("1. Remote open door\n");
    printf("2. List all cards in system\n");
    printf("3. Add/remove access\n");
    printf("4. Exit\n");
    printf("9. FAKE TEST SCAN CARD\n");

    int choice;
    char input[100];
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        return -1; 
    }
    return choice;
}

// Handles the user's menu choice and calls the corresponding function.
void processUserChoice(int choice, struct SystemState* state) {
    switch (choice) {
        case 1:
            remoteOpenDoor(state);
            waitForInput();
            break;
        case 2:
            listAllCards(state);
            waitForInput();
            break;
        case 3:
            addRemoveAccess(state);
            waitForInput();
            break;
        case 4:
            printf("Goodbye!\n");
            break;
        case 9:
            fakeCardScan(state);
            waitForInput();
            break;
        default:
            printf("Invalid choice. Please enter a number between 1 and 4 or 9.\n");
            waitForInput();
            break;
    }
}