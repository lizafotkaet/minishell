/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_substitute.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 15:25:06 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:12:40 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenize.h"
#include "pipeline.h"
#include "env.h"
#include "debug_alloc.h"

static bool	has_quotes(const char *s)
{
	while (*s)
	{
		if (*s == '\'' || *s == '"')
			return (true);
		s++;
	}
	return (false);
}

/*
** Substitute env vars in one word token.
** Returns 1 (keep), 0 (drop empty unquoted), -1 (error).
*/

static int	substitute_word(t_token *tok, t_env env)
{
	bool				unquoted;
	t_result_t_char_ptr	r;

	unquoted = !has_quotes(tok->content);
	r = substitute_env(tok->content, env);
	if (r.is_error)
		return (-1);
	if (r.value[0] == '\0' && unquoted)
	{
		free(r.value);
		if (tok->allocated)
			free(tok->content);
		return (0);
	}
	if (tok->allocated)
		free(tok->content);
	tok->content = r.value;
	tok->allocated = true;
	return (1);
}

/*
** Substitute environment variables in word tokens only.
** Removes words that expand to empty string and had no quotes.
*/

bool	substitute_all_envs(t_token_vector *tokens, t_env env)
{
	size_t	i;
	size_t	j;
	int		keep;

	if (tokens == NULL)
		return (false);
	i = 0;
	j = 0;
	while (i < tokens->size)
	{
		keep = 1;
		if (tokens->data[i].token_type == E_WORD)
			keep = substitute_word(&tokens->data[i], env);
		if (keep < 0)
			return (false);
		if (keep > 0)
			tokens->data[j++] = tokens->data[i];
		i++;
	}
	tokens->size = j;
	return (true);
}
