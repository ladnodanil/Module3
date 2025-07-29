#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#define LEN 80

void run_command(char **args)
{
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork failed");
        return;
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("exec failed");  
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

int main(int argc, char *argv[])
{
    char cmd[LEN];
    char *args[LEN];
    while (1)
    {
        printf(">>> ");
        if (!fgets(cmd, sizeof(cmd), stdin))
        {
            break;
        }
        cmd[strcspn(cmd, "\n")] = '\0';
        int i = 0;
        args[i] = strtok(cmd, " ");
        while (args[i] != NULL && i < LEN)
        {
            i++;
            args[i] = strtok(NULL, " ");
        }
        if (i == 0)
        {
            continue;
        }
        if (strcmp(args[0], "exit") == 0)
        {
            printf("Exiting...\n");
            break;
        }
        run_command(args);
    }

    return 0;
}