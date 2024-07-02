// Include the shell header file for necessary constants and function declarations
#include "shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>    // Include for time, localtime, and strftime functions
#include <unistd.h>  // Include for getcwd function on UNIX/Linux
#include <limits.h>  // Include for PATH_MAX constant

// Function to read a command from the user input
void read_command(char **cmd)
{
  // Define a character array to store the command line input
  char line[MAX_LINE];
  // Initialize count to keep track of the number of characters read
  int count = 0, i = 0;
  // Array to hold pointers to the parsed command arguments
  char *array[MAX_ARGS], *command_token;

  // Infinite loop to read characters until a newline or maximum line length is reached
  for (;;)
  {
    // Read a single character from standard input
    int current_char = fgetc(stdin);
    // Store the character in the line array and increment count
    line[count++] = (char)current_char;
    // If a newline character is encountered, break out of the loop
    if (current_char == '\n')
      break;
    // If the command exceeds the maximum length, print an error and exit
    if (count >= MAX_LINE)
    {
      printf("Command is too long, unable to process\n");
      exit(1);
    }
  }
  // Null-terminate the command line string
  line[count] = '\0';

  // If only the newline character was entered, return without processing
  if (count == 1)
    return;

  // Use strtok to parse the first token (word) of the command
  command_token = strtok(line, " \n");

  // Continue parsing the line into words and store them in the array
  while (command_token != NULL)
  {
    array[i++] = strdup(command_token);  // Duplicate the token and store it
    command_token = strtok(NULL, " \n"); // Get the next token
  }

  // Copy the parsed command and its parameters to the cmd array
  for (int j = 0; j < i; j++)
  {
    cmd[j] = array[j];
  }
  // Null-terminate the cmd array to mark the end of arguments
  cmd[i] = NULL;
}

// Function to display the shell prompt
void type_prompt()
{
  // Use a static variable to check if this is the first call to the function
  static int first_time = 1;
  if (first_time)
  {
    // Clear the screen on the first call
#ifdef _WIN32
    system("cls"); // Windows command to clear screen
#else
    system("clear"); // UNIX/Linux command to clear screen
#endif
    first_time = 0;
  }
  fflush(stdout); // Flush the output buffer

  // Get current time
    char time_str[100];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

    // Get current working directory
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        strcpy(cwd, "unknown");
    }

  printf("[%s] %s >> ", time_str, cwd);

  //printf("$$ ");  // Print the shell prompt
}


/*** This is array of functions, with argument char ***/
int (*builtin_commands_func[])(char **) = {
    &shell_cd,     // builtin_command_func[0]: cd 
    &shell_help,   // builtin_command_func[1]: help
    &shell_exit,   // builtin_command_func[2]: exit
    &shell_usage,  // builtin_command_func[3]: usage
    &list_env,     // builtin_command_func[4]: env
    &set_env_var,  // builtin_command_func[5]: setenv
    &unset_env_var // builtin_command_func[6]: unsetenv
};

// Helper function to figure out how many builtin commands are supported by the shell
int num_builtin_functions()
{
  return sizeof(builtin_commands) / sizeof(char *);
};

int shell_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  }
  else
  {
    if (chdir(args[1]) != 0)
    {
      perror("shell");
    }
  }
  return 1;
}

int shell_help(char **args)
{
  printf("Shell built-in commands:\n");
  for (int i = 0; i < num_builtin_functions(); i++)
  {
    printf("  %s\n", builtin_commands[i]);
  }
  return 1;
}

int shell_exit(char **args)
{ 
  return 0;
}

int shell_usage(char **args)
{
  printf("Usage: command [arguments]\n");
  printf("Type \"help\" to see a list of built-in commands.\n");
  return 1;
}

int list_env(char **args)
{
  extern char **environ;
  for (char **env = environ; *env != 0; env++)
  {
    char *thisEnv = *env;
    printf("%s\n", thisEnv);
  }
  return 1;
}

int set_env_var(char **args)
{
  if (args[1] == NULL || args[2] == NULL)
  {
    fprintf(stderr, "Usage: setenv [variable] [value]\n");
  }
  else
  {
    if (setenv(args[1], args[2], 1) != 0)
    {
      perror("shell");
    }
  }
  return 1;
}

int unset_env_var(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "Usage: unsetenv [variable]\n");
  }
  else
  {
    if (unsetenv(args[1]) != 0)
    {
      perror("shell");
    }
  }
  return 1;
}


// The main function where the shell's execution begins
int main(void)
{
  // Define an array to hold the command and its arguments
  char *cmd[MAX_ARGS];
  char *args[MAX_ARGS];
  int child_status;
  pid_t pid;
  char full_path[PATH_MAX];
  char cwd[1024];


  const char *filePath = "/Users/andrew/Downloads/programming-assignment-1-2024-ci05-nerdzunitez-main/.cseshellrc"; //hardcoded for now
  FILE *file = fopen(filePath, "r");
  if (file == NULL)
    {
        perror("Failed to open file");
        return EXIT_FAILURE;
    }

  char line[MAX_LINE_LENGTH];
  
  while (fgets(line, sizeof(line), file)){

    if (strncmp(line,"PATH=",5) == 0){
      char *path = line + 5;
      path[strlen(path) - 1] = '\0';
      setenv("PATH", path, 1);
    }
    else{
    pid = fork();
        if (pid == -1)
        {
            // If fork() returns -1, an error occurred
            perror("fork failed");
            _exit(EXIT_FAILURE);
        }
        else if (pid == 0){
          // Child process
          char *cmd = strtok(line, " \n"); // Tokenize the command
          char *args[256]; // Argument array for execvp
          int i = 0;
          while (cmd != NULL) {
              args[i++] = cmd;
              cmd = strtok(NULL, " \n"); // Continue tokenizing
          }
          args[i] = NULL; // Null-terminate the argument array
          execvp(args[0], args); // Execute the command

          fprintf(stderr, "command not found: %s", line); // Print if execvp fails
          perror(""); // Print the system error message
          
          _exit(EXIT_FAILURE); // Exit child process, should've used _exit(EXIT_SUCCESS) instead
          }
        else
        {
            // Parent process
            int status;
            waitpid(pid, &status, 0); // Wait for child process to finish
        }
    }
  }
  fclose(file);

  while (1) {
    type_prompt();     // Display the prompt
    read_command(args); // Read a command from the user

    // If the command is "exit", break out of the loop to terminate the shell
    if (args[0] == NULL) 
      continue; // Skips command exceution if the command is empty
    if (strcmp(args[0], "exit") == 0){
      break; // Exits the loop (and the shell) if the command is exit
    }
    // Loop through our command list and check if the commands exist in the builtin command list
    int is_builtin = 0;
    for (int command_index = 0; command_index < num_builtin_functions(); command_index++)
    {
      if (strcmp(args[0], builtin_commands[command_index]) == 0) // Assume args[0] contains the first word of the command
      {
      is_builtin = 1;
        (*builtin_commands_func[command_index])(args);
        break;
      }
    }

    if (is_builtin)
      continue;

    // Fork em'
    pid = fork();
    if (pid < 0){ 
      perror("Fork failed"); // If it fails somehow
      continue;
    }

    if (pid == 0) {  
    // Formulate the full path of the command to be executed
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/bin/%s", cwd, args[0]);
      } else {
        printf("Failed to get current working directory.");
        exit(1);
      }
    
      execv(full_path, cmd);

      // If execv returns, command execution has failed
      printf("Command %s not found\n", args[0]);
      exit(1);
    } else {
      waitpid(pid, &child_status, 0);
    }
  
    // Free the allocated memory for the command arguments before exiting
    for (int i = 0; args[i] != NULL; i++)
    {
      free(args[i]);
      args[i] = NULL;
    }
    memset(cwd, '\0', sizeof(cwd)); // clear the cwd array
  }  
  
  return 0;
}

