/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 01:42:43 by melkess           #+#    #+#             */
/*   Updated: 2025/03/19 11:54:14 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

static char	**extract_env_path(char **env, t_pipex pipex)
{
	size_t	i;
	size_t	x;
	size_t	y;
	size_t	len;
	char	*paths;

	i = 0;
	x = 0;
	y = 0;
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
	return (ft_split(paths, ':', x, y));
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
	pipex->cmd1 = ft_split(av[2], ' ', x, y);
	if (!pipex->cmd1)
		print_err("filling cmd1 failed", *pipex, 1);
	pipex->cmd2 = ft_split(av[3], ' ', x, y);
	if (!pipex->cmd2)
		print_err("filling cmd2 failed", *pipex, 1);
	pipex->outfile = av[4];
}

static void	ft_double_duptwo(int *io, int *pipefd, t_pipex pipex, int flag)
{
	if (flag)
	{
		if (dup2(0, io[0]) == -1 || dup2(1, io[1]) == -1)
		{
			(close(pipefd[0]), close(pipefd[1]));
			print_err("dup2 inbackup failed !!", pipex, 1);
		}
	}
	else
	{
		if (dup2(io[0], 0) == -1 || dup2(io[1], 1) == -1)
		{
			(close(pipefd[0]), close(pipefd[1]));
			print_err("dup2 in backup failed !!", pipex, 1);
		}
	}
}

// void	leaks(void)
// {
// 	system("leaks pipex");
// 	system("lsof -c pipex");
// }

int	main(int ac, char **av, char **env)
{
	t_pipex	pipex;
	int		pipefd[2];
	int		iobackup[2];
	int		forkids[2];
	int		status;

	// atexit(leaks);
	pipex.paths = NULL;
	if (ac != 5 || !av[1][0] || !av[4][0])
		print_err("./pipex file1 cmd1 cmd2 file2", pipex, 0);
	pipex.paths = extract_env_path(env, pipex);
	fill_pipex(&pipex, av);
	if (pipe(pipefd) == -1 || pipe(iobackup) == -1)
		print_err("pipe func failed !!", pipex, 1);
	ft_double_duptwo(iobackup, pipefd, pipex, 1);
	forking_executing(env, pipex, pipefd, forkids);
	(waitpid(forkids[0], NULL, 0), waitpid(forkids[1], &status, 0));
	ft_double_duptwo(iobackup, pipefd, pipex, 0);
	return (WEXITSTATUS(status));
}
