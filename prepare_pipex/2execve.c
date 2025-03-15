#include <unistd.h>
#include <stdio.h>

int main(int argc, char const *argv[])
{
    pid_t c = fork();
    char    *args[] = {"/bin/ls", NULL};
    char    *env[] = {NULL};
    if (c == 0)
        execve(args[0], args, env);
    else
        puts("Executing ls command...");
    return 0;          
}
