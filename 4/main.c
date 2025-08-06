#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define LEN 80

void pipeLine(char *cmdline)
{
    char *cmds[LEN];
    int n = 0;
    cmds[n] = strtok(cmdline, "|");
    while (cmds[n] && n < LEN - 1)
    {
        n++;
        cmds[n] = strtok(NULL, "|");
    }

    int in_fd = 0;
    int pipefd[2];

    for (int i = 0; i < n; i++)
    {
        char *args[LEN];
        int arg_count = 0;
        char *in_file = NULL;
        char *out_file = NULL;

        char *token = strtok(cmds[i], " \t");
        while (token)
        {
            if (strcmp(token, "<") == 0)
            {
                token = strtok(NULL, " \t");
                in_file = token;
            }
            else if (strcmp(token, ">") == 0)
            {
                token = strtok(NULL, " \t");
                out_file = token;
            }
            else
            {
                args[arg_count++] = token;
            }
            token = strtok(NULL, " \t");
        }
        args[arg_count] = NULL;

        if (i < n - 1)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                exit(1);
            }
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(1);
        }
        if (pid == 0)
        {
            // stdin
            if (in_fd != 0)
            {
                dup2(in_fd, 0);
                close(in_fd);
            }
            // stdout
            if (i < n - 1)
            {
                dup2(pipefd[1], 1);
                close(pipefd[0]);
                close(pipefd[1]);
            }
            // < input
            if (in_file)
            {
                int f = open(in_file, O_RDONLY);
                if (f == -1)
                {
                    perror("open input");
                    exit(1);
                }
                dup2(f, 0);
                close(f);
            }
            // > output
            if (out_file)
            {
                int f = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                if (f == -1)
                {
                    perror("open output");
                    exit(1);
                }
                dup2(f, 1);
                close(f);
            }
            execvp(args[0], args);
            perror("exec");
            exit(1);
        }
        else
        {
            // Родитель
            wait(NULL);
            if (in_fd != 0)
                close(in_fd);
            if (i < n - 1)
            {
                close(pipefd[1]);
                in_fd = pipefd[0];
            }
        }
    }
}

int main(int argc, char *argv[])
{
    char cmd[LEN];
    while (1)
    {
        printf(">>> ");
        if (!fgets(cmd, sizeof(cmd), stdin))
            break;
        cmd[strcspn(cmd, "\n")] = '\0';
        if (strcmp(cmd, "exit") == 0)
            break;
        if (strlen(cmd) == 0)
            continue;
        pipeLine(cmd);
    }
    return 0;
}