#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Modified function to apply high contrast color scheme based on user choice
void applyHighContrast(const char* terminalType, int choice) {
    char* fgColor;
    char* bgColor;

    switch (choice) {
        case 1:
            fgColor = "#FFFFFF"; // White foreground
            bgColor = "#000000"; // Black background
            break;
        case 2:
            fgColor = "#FFFF00"; // Bright yellow foreground
            bgColor = "#00008B"; // Dark blue background
            break;
        case 3:
            fgColor = "#b84011"; // Pink foreground
            bgColor = "#11b83d"; // Green background
            break;
        default:
            printf("Invalid choice. Defaulting to scheme 1.\n");
            fgColor = "#FFFFFF"; // White foreground
            bgColor = "#000000"; // Black background
    }

    if (strstr(terminalType, "xterm") != NULL) {
        char command[100];
        sprintf(command, "echo -e '\\033]10;%s\\007\\033]11;%s\\007'", fgColor, bgColor);
        system(command);
    } else {
        printf("Unsupported terminal type. High contrast mode not applied.\n");
    }
}

// Modified function to toggle high contrast mode with choice
void toggleHighContrast(int choice) {
    char* terminalType = getenv("TERM");
    if (terminalType != NULL) {
        applyHighContrast(terminalType, choice);
    } else {
        printf("Terminal type not detected. Please ensure you are running this in a terminal emulator.\n");
    }
}

int main() {
    int choice;
    printf("Choose a color scheme (1, 2, or 3): ");
    if (scanf("%d", &choice) == 1) {
        toggleHighContrast(choice);
    } else {
        printf("Invalid input. Please enter a number (1, 2, or 3).\n");
    }
    return 0;
}