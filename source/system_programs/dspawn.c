#include "system_program.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>

void daemonize() {

    pid_t pid, sid;

    pid = fork();
    if (pid < 0 ){
        exit(EXIT_FAILURE);
        
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }else{   

        // Child process
        sid = setsid();

        if (sid < 1){
            exit(EXIT_FAILURE);
        }

        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN);

         //child process becomes session leader, to lose controlling TTY
        pid = fork();

        if (pid < 0){
            exit(EXIT_FAILURE);
        }
        if (pid > 0){
            exit(EXIT_SUCCESS);
        }

        umask(0); //set new file permissions 

        chdir("/"); //change working directory to root

        /* Close all open file descriptors */
        int x;
        for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
        {
            close (x);
        }

        /*
        * Attach file descriptors 0, 1, and 2 to /dev/null. */
        int fd0 = open("/dev/null", O_RDWR);
        int fd1 = dup(0);
        int fd2 = dup(0);

        if (fd0 == -1 || fd1 == -1 || fd2 == -1)
        {   
            // Handle the error if any of the file descriptors couldn't be opened
            perror("open or dup");
            exit(EXIT_FAILURE);
        }

    } 
    
}

char output_file_path[PATH_MAX];

static int daemon_work()
{
    int num = 0;
    FILE *fptr;
    char buffer[1024];
    char *cwd;

    // write PID of daemon in the beginning
    fptr = fopen(output_file_path, "a");
    if (fptr == NULL)
    {
        return EXIT_FAILURE;
    }

    fprintf(fptr, "Daemon process running with PID: %d, PPID: %d, opening logfile with FD %d\n", getpid(), getppid(), fileno(fptr));

    // then write cwd
    cwd = getcwd(buffer, sizeof(buffer));
    if (cwd == NULL)
    {
        perror("getcwd() error");
        return 1;
    }

    fprintf(fptr, "Current working directory: %s\n", cwd);
    fclose(fptr);

    while (1)
    {
        fptr = fopen(output_file_path, "a");
        if (fptr == NULL)
        {
            return EXIT_FAILURE;
        }

        fprintf(fptr, "PID %d Daemon writing line %d to the file.\n", getpid(), num);
        num++;

        fclose(fptr);

        sleep(10);

        if (num == 10) // terminate after 10 counts
            break;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    // Setup path
    if (getcwd(output_file_path, sizeof(output_file_path)) == NULL)
    {
        perror("getcwd() error, exiting now.");
        return 1;
    }
    strcat(output_file_path, "/dspawn.log");

    daemonize();
    daemon_work();
    return 0;
}