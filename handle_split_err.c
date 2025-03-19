/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_split_err.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkess <melkess@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/15 08:39:37 by melkess           #+#    #+#             */
/*   Updated: 2025/03/19 11:47:56 by melkess          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

void	print_err(char *msg, t_pipex pipex, int flag)
{
	flag = 0;
	if (flag)
	{
		free_double(pipex.paths);
		free_double(pipex.cmd1);
		free_double(pipex.cmd2);
	}
	perror(msg);
	exit(1);
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

char	**ft_split(char *s, char c, int i, int j)
{
	char	**splitedstr;
	char	*tmp;
	int		nbrwords;

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
			return (ft_free_splitedstr(splitedstr, j), NULL);
		if (++j != nbrwords)
		{
			tmp = splitedstr[j];
			splitedstr[j] = ft_strjoin(splitedstr[j], "/");
			free(tmp);
		}
	}
	return (splitedstr[j] = NULL, splitedstr);
}
