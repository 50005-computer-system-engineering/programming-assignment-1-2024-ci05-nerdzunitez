#include "system_program.h"
#include <sys/utsname.h>
#include <sys/sysctl.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void get_CPUINFO(); //function to get CPU information
void get_UPTIME(); //function to get uptime

int main(){
    struct utsname unameData;
    if (uname(&unameData)< 0){
        perror("uname");
        exit(EXIT_FAILURE); 
    } 
    else{
    printf("OS: %s\n", unameData.sysname);
    printf("Kernel: %s\n", unameData.release);
    get_UPTIME();
    printf("User: %s\n", getenv("USER"));
    get_CPUINFO();
   }
    return 0;
}

void get_UPTIME() {
    FILE *fp;
    char buffer[1024];
    long uptime_seconds = 0;
    int days = 0, hours = 0, minutes = 0;
    

    fp = popen("uptime", "r"); // popen command to run uptime command and read the output of uptime 
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }
    
    if (fgets(buffer, sizeof(buffer), fp) != NULL) { // using fgets to read the output of fp and store it in buffer
        
        char *days_pointer = strstr(buffer, "days");

        if (days_pointer != NULL) { //check if uptime contains days
            if (isdigit(*(days_pointer - 3))){ //if days is a two digit number
                char temp[3] = {0};
                strncpy(temp, days_pointer - 3, 2); //store the days in temp
                days = atoi(temp);
                
                char *colon_after_days_pointer = strstr(days_pointer, ":"); //find the colon after days
                char temp2[3] = {0};
                strncpy(temp2, colon_after_days_pointer -2, 2); //store the hours in temp2
                hours = atoi(temp2);

                char temp3[3] = {0};
                strncpy(temp3, colon_after_days_pointer + 1, 2); //store the minutes in temp3
                minutes = atoi(temp3);

            } else{ //if days is a single digit number
                char temp[2] = {0};
                strncpy(temp,days_pointer -2, 1); //store the days in temp
                days = atoi(temp);

                char *colon_after_days_pointer = strstr(days_pointer, ":"); //find the colon after days
                char temp2[3] = {0};
                strncpy(temp2, colon_after_days_pointer - 2, 2); // store the hours in temp2
                hours = atoi(temp2);

                char temp3[3] = {0};
                strncpy(temp3, colon_after_days_pointer + 1, 2); //store the minutes in temp3
                minutes = atoi(temp3);
            }
            
        }else { //if not, uptime only has hours and minutes
            char *up_pointer = strstr(buffer, "up"); //first colon is current time
            char *second_colon_pointer = strstr(up_pointer, ":"); //find the second colon for uptime

            char temp[3] = {0};
            strncpy(temp, second_colon_pointer - 2, 2); //store the hours in temp
            hours = atoi(temp);

            char temp2[3] = {0};
            strncpy(temp2, second_colon_pointer + 1, 2); //store the minutes in temp2
            minutes = atoi(temp2);
            
            printf("%d minutes\n", minutes);
            }
        } 
    pclose(fp);
    printf("Uptime: %d days, %d hours, %d minutes\n", days, hours, minutes);

}

void get_CPUINFO() {
    FILE *fp = NULL;
    char buffer[1024];

    // Use sysctl command to get CPU brand string on macOS
    fp = popen("sysctl -n machdep.cpu.brand_string", "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        exit(1);
    }
    // Read the output of the command
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("CPU: %s", buffer); // Print the CPU information
    } 
    // Close the file pointer
    pclose(fp);
}