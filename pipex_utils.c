/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/19 11:40:38 by melkess           #+#    #+#             */
/*   Updated: 2025/03/21 18:21:24 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	free_double(char **d)
{
	size_t	i;

	i = 0;
	while (d && d[i])
		free(d[i++]);
	free(d);
}

static void	exec_helper(char **paths, char **cmd, char **env, t_pipex pipex)
{
	size_t	i;
	char	*tmp;

	i = 0;
	while (paths && paths[i])
	{
		tmp = paths[i];
		paths[i] = ft_strjoin(paths[i], "/");
		free(tmp);
		tmp = paths[i];
		paths[i] = ft_strjoin(paths[i], cmd[0]);
		free(tmp);
		if (!access(paths[i], X_OK) && cmd[0])
		{
			if (execve(paths[i], cmd, env) == -1)
			{
				ft_dup(pipex.pipefd, pipex, 0, -1);
				print_err("Execve total failed !", pipex, 1);
			}
		}
		i++;
	}
}

static void	execute_first_child(char **env, t_pipex pipex, int *pipefd)
{
	int	fd;
	int	io[2];

	close(pipefd[0]);
	fd = open(pipex.infile, O_RDONLY);
	if (fd == -1)
		(close(pipefd[1]), print_err("Failed to open file", pipex, 1));
	ft_dup(io, pipex, 1, fd);
	if (dup2(fd, 0) == -1 || dup2(pipefd[1], 1) == -1)
		(close(pipefd[1]), close(fd), print_err("dup2 failed", pipex, 1));
	(close(fd), close(pipefd[1]));
	if ((!access(pipex.cmd1[0], X_OK) || !pipex.paths)
		&& ft_strchr(pipex.cmd1[0], '/'))
	{
		if (execve(pipex.cmd1[0], pipex.cmd1, env) == -1)
			(ft_dup(io, pipex, 0, fd), print_err("Execve1 failed !", pipex, 1));
	}
	else if (!ft_strchr(pipex.cmd1[0], '/'))
	{
		pipex.pipefd[0] = io[0];
		pipex.pipefd[1] = io[1];
		exec_helper(pipex.paths, pipex.cmd1, env, pipex);
	}
	write(2, pipex.cmd1[0], ft_strlen(pipex.cmd1[0]));
	(write(2, " : command not found\n", 21), ft_dup(io, pipex, 0, fd));
}

static void	execute_sec_child(char **env, t_pipex pipex, int *pipefd)
{
	int	fd;
	int	io[2];

	close(pipefd[1]);
	fd = open(pipex.outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
		(close(pipefd[0]), print_err("Failed to open file", pipex, 1));
	ft_dup(io, pipex, 1, fd);
	if (dup2(pipefd[0], 0) || dup2(fd, 1) == -1)
		(close(pipefd[0]), close(fd), print_err("dup2 failed", pipex, 1));
	(close(pipefd[0]), close(fd));
	if ((!access(pipex.cmd2[0], X_OK) || !pipex.paths)
		&& ft_strchr(pipex.cmd2[0], '/'))
	{
		if (execve(pipex.cmd2[0], pipex.cmd2, env) == -1)
			(ft_dup(io, pipex, 0, fd), print_err("Execve2 failed !", pipex, 1));
	}
	else if (!ft_strchr(pipex.cmd2[0], '/'))
	{
		pipex.pipefd[0] = io[0];
		pipex.pipefd[1] = io[1];
		exec_helper(pipex.paths, pipex.cmd2, env, pipex);
	}
	write(2, pipex.cmd2[0], ft_strlen(pipex.cmd2[0]));
	(write(2, " : command not found\n", 21), ft_dup(io, pipex, 0, fd));
}

void	forking_executing(char **env, t_pipex pipex, int *fork_ids)
{
	fork_ids[0] = fork();
	if (fork_ids[0] == -1)
	{
		(close(pipex.pipefd[0]), close(pipex.pipefd[1]));
		print_err("fork func failes !!", pipex, 1);
	}
	if (fork_ids[0] == 0)
	{
		execute_first_child(env, pipex, pipex.pipefd);
		exit(0);
	}
	fork_ids[1] = fork();
	if (fork_ids[1] == -1)
	{
		(close(pipex.pipefd[0]), close(pipex.pipefd[1]));
		print_err("fork func failes !!", pipex, 1);
	}
	if (fork_ids[1] == 0)
	{
		execute_sec_child(env, pipex, pipex.pipefd);
		exit(1);
	}
	(close(pipex.pipefd[0]), close(pipex.pipefd[1]));
}
