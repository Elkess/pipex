/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 01:42:43 by melkess           #+#    #+#             */
/*   Updated: 2025/03/15 09:38:23 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	**extract_env_path(char **env, t_pipex pipex)
{
	size_t	i;
	size_t	len;
	char	*paths;

	if (!env || !*env)
		print_err("Environment variable is empty !!", pipex, 0);
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
	return (ft_split(paths, ':'));
}

static void	fill_pipex(t_pipex *pipex, char **av)
{
	size_t	i;

	i = 1;
	pipex->infile = av[1];
	pipex->cmd1 = NULL;
	pipex->cmd2 = NULL;
	pipex->cmd1 = ft_split(av[2], ' ');
	if (!pipex->cmd1)
		print_err("filling cmd1 failed", *pipex, 1);
	pipex->cmd2 = ft_split(av[3], ' ');
	if (!pipex->cmd2)
		print_err("filling cmd2 failed", *pipex, 1);
	pipex->outfile = av[4];
}

static void	execute_child(char **env, t_pipex pipex, int *pipefd)
{
	size_t	i;
	int		fd;
	char	*tmp;

	close(pipefd[0]);
	fd = open(pipex.infile, O_RDONLY);
	if (fd == -1)
		(close(pipefd[1]), print_err("Failed to open file", pipex, 1));
	if (dup2(fd, 0) == -1 || dup2(pipefd[1], 1) == -1)
		(close(pipefd[1]), close(fd), print_err("dup2 failed", pipex, 1));
	(close(fd), close(pipefd[1]));
	i = 0;
	while (pipex.paths[i])
	{
		tmp = pipex.paths[i];
		pipex.paths[i] = ft_strjoin(pipex.paths[i], "/");
		free(tmp);
		tmp = pipex.paths[i];
		pipex.paths[i] = ft_strjoin(pipex.paths[i], pipex.cmd1[0]);
		free(tmp);
		if (execve(pipex.paths[i], pipex.cmd1, env) == -1)
			i++;
	}
	free_double(pipex.cmd1);
	free_double(pipex.paths);
}

static void	execute_parent(char **env, t_pipex pipex, int *pipefd)
{
	size_t	i;
	int		fd;
	char	*tmp;

	close(pipefd[1]);
	fd = open(pipex.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		(close(pipefd[0]), print_err("Failed to open file", pipex, 1));
	if (dup2(pipefd[0], 0) || dup2(fd, 1) == -1)
		(close(pipefd[0]), close(fd), print_err("dup2 failed", pipex, 1));
	(close(pipefd[0]), close(fd));
	i = 0;
	while (pipex.paths[i])
	{
		tmp = pipex.paths[i];
		pipex.paths[i] = ft_strjoin(pipex.paths[i], "/");
		free(tmp);
		tmp = pipex.paths[i];
		pipex.paths[i] = ft_strjoin(pipex.paths[i], pipex.cmd2[0]);
		free(tmp);
		if (execve(pipex.paths[i], pipex.cmd2, env) == -1)
			i++;
	}
	free_double(pipex.cmd2);
	free_double(pipex.paths);
}

int	main(int ac, char **av, char **env)
{
	t_pipex	pipex;
	int		pipefd[2];
	pid_t	id;

	pipex.paths = NULL;
	if (ac != 5 || !av[1][0] || !av[2][0] || !av[3][0] || !av[4][0])
		print_err("./pipex file1 cmd1 cmd2 file2", pipex, 0);
	pipex.paths = extract_env_path(env, pipex);
	fill_pipex(&pipex, av);
	if (pipe(pipefd) == -1)
		print_err("pipe func failed !!", pipex, 1);
	id = fork();
	if (id == -1)
	{
		(close(pipefd[0]), close(pipefd[1]));
		print_err("fork func failes !!", pipex, 1);
	}
	if (!id)
		execute_child(env, pipex, pipefd);
	execute_parent(env, pipex, pipefd);
	free_double(pipex.paths);
}
