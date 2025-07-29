#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int is_int(char *str)
{
    if (*str == '\0')
        return 0;
    while (*str)
    {
        if (!isdigit(*str))
            return 0;
        str++;
    }
    return 1;
}

int is_float(char *str)
{
    int flag = 0;
    if (*str == '\0')
        return 0;

    while (*str)
    {
        if (*str == '.')
        {
            if (flag)
                return 0;
            flag = 1;
        }
        else if (!isdigit(*str))
        {
            return 0;
        }
        str++;
    }
    return flag;
}

void print_proc(size_t start, size_t step, int argc, char *argv[])
{
    for (size_t i = start; i < (size_t)argc; i += step)
    {
        if (is_int(argv[i]))
        {
            int num = atoi(argv[i]);
            printf("%d\n", num * 2);
        }
        else if (is_float(argv[i]))
        {
            float num = atof(argv[i]);
            printf("%f\n", num * 2);
        }
        else
        {
            printf("%s\n", argv[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    pid_t pid;
    int rv;
    switch (pid = fork())
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
    case 0: // потомок
            print_proc(1, 2, argc, argv);
            exit(0);
    default: // родитель
        print_proc(2, 2, argc, argv);
        int status;
        wait(&status);
        if (WIFEXITED(status))
        {
            printf("Child exited with status %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}