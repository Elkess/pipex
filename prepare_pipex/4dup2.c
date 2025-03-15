#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newstr;
	char	*head;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (strdup(s2));
	if (!s2)
		return (strdup(s1));
	newstr = (char *)malloc(strlen(s1) + strlen(s2) +1);
	if (!newstr)
		return (NULL);
	head = newstr;
	while (*s1)
		*newstr++ = *s1++;
	while (*s2)
		*newstr++ = *s2++;
	*newstr = '\0';
	return (head);
}

int main(int argc, char *argv[])
{
	if (argc == 4)
	{
	int pipe1[2];
	if (pipe(pipe1) == -1) { perror("pipe failed"); exit(1); }
	int id = fork();
	int pid = getpid();
	if ((id) == -1) { perror("fork failed"); exit(1); }
	printf("%d\n", pid);
    if (id > 0)
    {
		close(pipe1[0]);
		if (dup2(pipe1[1], 1) < 0) {puts("ERRRRR");}
		close(pipe1[1]);
        char *cmd = "/bin/";
        cmd = ft_strjoin(cmd, argv[1]);
        char *args[] = {argv[1], NULL} ;
        execve(cmd, args, NULL);
    }
	close(pipe1[1]);
	if (dup2(pipe1[0], 0) < 0) {puts("ERRRRR");}
	close(pipe1[0]);
    char *cmd = "/usr/bin/";
    cmd = ft_strjoin(cmd, argv[2]);
    char *args[] = {argv[2], argv[3], NULL};
    execve(cmd, args, NULL);	
}
}
