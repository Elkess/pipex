#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    int pipe1[2]; // p -> c
    pipe(pipe1);
    int id = fork();
    if (id < 0) return 1;
    if (id == 0)
    {
        close(pipe1[1]);
        char *x = malloc(13);
        read(pipe1[0], x, 13);
        write(1, x, 13);
        close(pipe1[0]);
    }
    else
    {
        close(pipe1[0]);
        char *s = "Hello, child!";
        write(pipe1[1], s, 13);
        close(pipe1[1]);
    }
}