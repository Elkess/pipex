#include  <stdio.h>
#include <unistd.h>

int  main(void)
{
    int id = fork();
    printf("%d\n", getpid());
    if (id == 0 )
    {
        printf("I am the child process %d \n", id);
        printf("* %d\n", getpid());
    }
    else
       printf("I am the parent process: %d\n", id);
}