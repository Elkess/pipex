/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 01:42:43 by melkess           #+#    #+#             */
/*   Updated: 2025/03/21 18:15:48 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	**extract_env_path(char **env)
{
	size_t	i;
	size_t	len;
	char	*paths;
	char	**splited;

	if (!env || !*env)
		return (ft_split(ft_strdup("."), ':', 0, 0));
	i = 0;
	paths = NULL;
	while (env && env[i])
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
	if (!*paths)
		paths = ft_strdup(".");
	splited = ft_split(paths, ':', 0, 0);
	free(paths);
	return (splited);
}

static void	fill_pipex(t_pipex *pipex, char **av)
{
	size_t	i;
	size_t	x;
	size_t	y;

	i = 1;
	x = 0;
	y = 0;
	pipex->infile = av[1];
	pipex->cmd1 = NULL;
	pipex->cmd2 = NULL;
	pipex->cmd1 = ft_split(av[2], ' ', x, y);
	if (!pipex->cmd1)
		print_err("filling cmd1 failed", *pipex, 1);
	pipex->cmd2 = ft_split(av[3], ' ', x, y);
	if (!pipex->cmd2)
		print_err("filling cmd2 failed", *pipex, 1);
	pipex->outfile = av[4];
}

void	ft_dup(int *io, t_pipex pipex, int flag, int fd)
{
	if (flag)
	{
		io[0] = dup(0);
		io[1] = dup(1);
		if (io[0] == -1 || io[1] == -1)
		{
			close(fd);
			(close(pipex.pipefd[0]), close(pipex.pipefd[1]));
			(close(io[0]), close(io[1]));
			print_err("dup2 pre backup failed !!", pipex, 1);
		}
	}
	else
	{
		if (dup2(io[0], 0) == -1 || dup2(io[1], 1) == -1)
		{
			(close(io[0]), close(io[1]));
			print_err("dup2 in backup failed !!", pipex, 1);
		}
		(close(io[0]), close(io[1]));
	}
}

int	main(int ac, char **av, char **env)
{
	t_pipex	pipex;
	int		fork_ids[2];
	int		status;

	pipex.cmd1 = NULL;
	if (ac != 5 || !av[1][0] || !av[4][0])
		print_err("./pipex file1 cmd1 cmd2 file2", pipex, 0);
	pipex.paths = extract_env_path(env);
	fill_pipex(&pipex, av);
	if (pipe(pipex.pipefd) == -1)
		print_err("pipe func failed !!", pipex, 1);
	forking_executing(env, pipex, fork_ids);
	(waitpid(fork_ids[0], NULL, 0), waitpid(fork_ids[1], &status, 0));
	free_double(pipex.cmd1);
	free_double(pipex.cmd2);
	free_double(pipex.paths);
	return (WEXITSTATUS(status));
}
