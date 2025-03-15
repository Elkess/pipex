/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   5pipex_mandatory.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 01:42:43 by melkess           #+#    #+#             */
/*   Updated: 2025/03/15 08:27:25 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void	print_err(char *msg)
{
	perror(msg);
	exit(1);
}

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}

char	*ft_strdup(const char *s1)
{
	char	*s;
	size_t	i;

	i = 0;
	while (s1[i])
		i++;
	s = (char *)malloc(i +1);
	if (!s)
		return (NULL);
	i = 0;
	while (s1[i])
	{
		s[i] = s1[i];
		i++;
	}
	s[i] = '\0';
	return (s);
}

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*substr;
	size_t	i;
	size_t	lenfromstart;

	if (!s)
		return (NULL);
	if (start > ft_strlen(s))
		return (ft_strdup(""));
	lenfromstart = ft_strlen(s + start);
	if (len > lenfromstart)
		len = lenfromstart;
	substr = malloc(len +1);
	if (!substr)
		return (NULL);
	i = 0;
	while (i < len)
	{
		substr[i] = s[start + i];
		i++;
	}
	substr[i] = '\0';
	return (substr);
}

static int	ft_nbr_words(const char *s, char c)
{
	int	i;
	int	n;

	i = 0;
	n = 0;
	if (s[0] != c && s[0] != '\0')
		n++;
	while (s[i])
	{
		if ((s[i] == c && s[i +1] != c && s[i +1] != '\0'))
			n++;
		i++;
	}
	return (n);
}

static char	*ft_one_word(char *s, char c, int *i)
{
	int	start;
	int	end;

	while (s[*i] && s[*i] == c)
		(*i)++;
	start = *i;
	while (s[*i] && s[*i] != c)
		(*i)++;
	end = *i;
	return (ft_substr(s, start, end - start));
}

static void	ft_free_splitedstr(char **splitedstr, int j)
{
	while (j > 0)
		free(splitedstr[--j]);
	free(splitedstr);
}

char	**ft_split(char *s, char c)
{
	char	**splitedstr;
	int		nbrwords;
	int		i;
	int		j;

	if (!s)
		return (NULL);
	nbrwords = ft_nbr_words(s, c);
	splitedstr = (char **) malloc((nbrwords +1) * sizeof(char *));
	if (!splitedstr)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i] && j < nbrwords)
	{
		splitedstr[j] = ft_one_word(s, c, &i);
		if (!splitedstr[j])
		{
			(ft_free_splitedstr(splitedstr, j));
			return (NULL);
		}
		j++;
	}
	(splitedstr[j] = NULL);
	return (splitedstr);
}

char	*extract_env_path(char **env)
{
	size_t	i;
	size_t	len;
	char	*paths;

	if (!env || !*env)
		print_err("Environment variable is empty !!");
	i = 0;
	paths = NULL;
	while (env[i])
	{
		len = ft_strlen(env[i]);
		if (env[i][0] == 'P' && env[i][1] == 'A'
			&& env[i][2] == 'T' && env[i][3] == 'H')
		{
			len -= 5;
			paths = ft_substr(env[i], 5, len);
		}
		i++;
	}
	return (paths);
}

typedef struct s_pipex
{
	char	*infile;
	char	**cmd1;
	char	**cmd2;
	char	*outfile;

}	t_pipex;

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*newstr;
	char	*head;

	if (!s1 && !s2)
		return (NULL);
	if (!s1)
		return (ft_strdup(s2));
	if (!s2)
		return (ft_strdup(s1));
	newstr = (char *)malloc(ft_strlen(s1) + ft_strlen(s2) +1);
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

char	**filter_paths(char **env)
{
	char	*paths;

	paths = extract_env_path(env);
	return (ft_split(paths, ':'));
}

void	fill_pipex(t_pipex *pipex, char **av)
{
	size_t	i;

	i = 1;
	pipex->infile = av[1];
	pipex->cmd1 = ft_split(av[2], ' ');
	pipex->cmd2 = ft_split(av[3], ' ');
	pipex->outfile = av[4];
}
// void	leaks(void)
// {
// 	system("leaks a.out");
// 	system("lsof -c a.out");
// }

void	execute_child(char **env, t_pipex pipex, int *pipefd, char	**paths)
{
	size_t	i;
	int		fd;

	i = 0;
	close(pipefd[0]);
	fd = open(pipex.infile, O_RDONLY);
	if (fd == -1)
		(close(pipefd[1]), print_err("Failed to open file"));
	if (dup2(fd, 0) == -1 || dup2(pipefd[1], 1) == -1)
		(close(pipefd[1]), close(fd), print_err("dup2 failed"));
	close(fd);
	while (paths[i])
	{
		paths[i] = ft_strjoin(paths[i], "/");
		paths[i] = ft_strjoin(paths[i], pipex.cmd1[0]);
		if (execve(paths[i], pipex.cmd1, env) == -1)
			i++;
	}
}

void	execute_parent(char **env, t_pipex pipex, int *pipefd, char	**paths)
{
	size_t	i;
	int		fd;

	fd = open(pipex.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		(close(pipefd[1]), print_err("Failed to open file"));
	close(pipefd[1]);
	if (dup2(pipefd[0], 0) || dup2(fd, 1) == -1)
		(close(pipefd[1]), close(fd), print_err("dup2 failed"));
	close(pipefd[0]);
	paths[1] = ft_strjoin(paths[1], "/");
	paths[1] = ft_strjoin(paths[1], pipex.cmd2[0]);
	if (execve(paths[1], pipex.cmd2, NULL) == -1)
		(close(fd), print_err("execve func failed !!"));
}

int	main(int ac, char **av, char **env)
{
	// atexit(leaks);
	t_pipex	pipex;
	int		pipefd[2];
	pid_t	id;
	int		fd;
	char	**paths;

	if (ac != 5 || !av[1][0] || !av[2][0] || !av[3][0] || !av[4][0])
		print_err("./pipex file1 cmd1 cmd2 file2");
	paths = filter_paths(env);
	fill_pipex(&pipex, av);
	if (pipe(pipefd) == -1)
		print_err("pipe func failed !!");
	id = fork();
	if (id == -1)
		print_err("fork func failes !!");
	if (!id)
		execute_child(env, pipex, pipefd, paths);
	execute_parent(env, pipex, pipefd, paths);
}
