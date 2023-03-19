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