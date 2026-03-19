/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/28 19:11:35 by liza              #+#    #+#             */
/*   Updated: 2026/02/28 20:16:09 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "include/minishell.h"

char	*ft_substr(char const *s, unsigned int start, size_t len);

static int	word_count(const char *s, char c)
{
	int	arr_len;
	int	flag;

	arr_len = 0;
	flag = 0;
	while (*s)
	{
		if (*s != c && flag == 0)
		{
			arr_len++;
			flag = 1;
		}
		else if (*s == c)
			flag = 0;
		s++;
	}
	return (arr_len);
}

static int	ft_word_len(const char *s, char c)
{
	int	wordlen;

	wordlen = 0;
	while (*s && *s != c)
	{
		if (*s == c)
			return (wordlen);
		wordlen++;
		s++;
	}
	return (wordlen);
}

static void	free_all(char	**arr, int arr_len)
{
	int	i;

	i = 0;
	while (i < arr_len)
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	int		arr_len;
	int		word_len;
	int		i;

	arr_len = word_count(s, c);
	arr = malloc(sizeof(char *) * (arr_len + 1));
	if (arr == NULL)
		return (NULL);
	word_len = 0;
	i = 0;
	while (i < arr_len)
	{
		while (*s == c)
			s++;
		word_len = ft_word_len(s, c);
		arr[i] = ft_substr(s, 0, word_len);
		s += word_len;
		word_len = 0;
		if (!arr[i])
			return (free_all(arr, i), NULL);
		i++;
	}
	arr[arr_len] = NULL;
	return (arr);
}

typedef struct s_command
{
	bool			has_command;
	bool			has_redirects;
	char			*name;
	size_t			argc;
	char			**argv;
	pid_t			pid;
	t_redirect		*redirects;
	t_command		*next;
}	t_command;

int	main(int ac, char *av)
{
	t_command	cmds;
	char		**tokens;

	tokens = ft_split()

	
}