#include <stdio.h>
#include <stdlib.h>

int main() {
    char *cmd = "ps -efj | grep dspawn | grep -Ev 'tty|pts' > /tmp/dspawn_procs.txt";
    system(cmd); // Execute command and redirect output to a file

    FILE *file = fopen("/tmp/dspawn_procs.txt", "r"); // Open the file for reading
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    int count = 0;
    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        count++; // Count each line in the file
    }

    fclose(file); // Close the file

    printf("Number of dspawn daemon processes running: %d\n", count);

    // Optionally, delete the temporary file
    remove("/tmp/dspawn_procs.txt");

    return EXIT_SUCCESS;
}