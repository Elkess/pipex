/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 08:32:32 by melkess           #+#    #+#             */
/*   Updated: 2025/03/21 17:46:16 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include <unistd.h>
# include <fcntl.h>
# include <stdlib.h>
# include <stdio.h>

typedef struct s_pipex
{
	char	**paths;
	char	*infile;
	char	**cmd1;
	char	**cmd2;
	char	*outfile;
	int		pipefd[2];

}	t_pipex;

void	forking_executing(char **env, t_pipex pipex, int *forkids);
void	print_err(char *msg, t_pipex pipex, int flag);
size_t	ft_strlen(const char *s);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	**ft_split(char *s, char c, int i, int j);
char	*ft_strchr(const char *s, int c);
char	*ft_strdup(const char *s1);
void	free_double(char **d);
void	ft_dup(int *io, t_pipex pipex, int flag, int fd);

#endif