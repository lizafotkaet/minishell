/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize_build.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:03:31 by liza              #+#    #+#             */
/*   Updated: 2026/03/19 13:03:32 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "tokenize.h"
#include "pipeline.h"
#include "libft.h"
#include "debug_alloc.h"

/*
** Append a single token to `vec` for the operator at `it`.
** Returns the advanced pointer, or NULL on failure.
*/
static const char	*append_operator(const char *it, t_token_vector *vec)
{
	int		len;
	t_token	tok;

	len = operator_length(it);
	tok.content = ft_substr(it, 0, len);
	if (!tok.content)
		return (NULL);
	tok.token_type = operator_type(it, len);
	tok.allocated = true;
	if (!m_token_vector_append(vec, tok))
	{
		FREE(tok.content);
		return (NULL);
	}
	return (it + len);
}

/*
** Append a word token to `vec` starting at `it`.
** Returns the advanced pointer, or NULL on failure.
*/
static const char	*append_word(const char *it, t_token_vector *vec)
{
	RESULT(t_const_char_ptr)	word;
	t_token						tok;

	word = skip_word(it);
	if (word.is_error)
		return (NULL);
	tok.content = ft_substr(it, 0, word.value - it);
	if (!tok.content)
		return (NULL);
	tok.token_type = E_WORD;
	tok.allocated = true;
	if (!m_token_vector_append(vec, tok))
	{
		FREE(tok.content);
		return (NULL);
	}
	return (word.value);
}

/*
** Splits `input` into a t_token_vector of typed tokens.
*/
RESULT(t_token_vector)	tokenize_string(const char *input)
{
	const char				*it;
	RESULT(t_token_vector)	vr;
	t_token_vector			vec;

	if (input == NULL)
		return (ERROR(t_token_vector));
	vr = m_token_vector_new();
	if (vr.is_error)
		return (ERROR(t_token_vector));
	vec = vr.value;
	it = input;
	while (*it != '\0')
	{
		it = skip_consecutive_whitespaces(it);
		if (*it == '\0')
			break ;
		if (is_operator_char(*it))
			it = append_operator(it, &vec);
		else
			it = append_word(it, &vec);
		if (it == NULL)
			return (m_token_vector_free(&vec), ERROR(t_token_vector));
	}
	return (SUCCESS(t_token_vector, vec));
}

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
** Substitute environment variables in word tokens only.
** Removes words that expand to empty string and had no quotes.
*/
bool	substitute_all_envs(t_token_vector *tokens, t_env env)
{
	size_t				i;
	size_t				j;
	bool				unquoted;
	RESULT(t_char_ptr)	r;

	if (tokens == NULL)
		return (false);
	i = 0;
	j = 0;
	while (i < tokens->size)
	{
		if (tokens->data[i].token_type == E_WORD)
		{
			unquoted = !has_quotes(tokens->data[i].content);
			r = substitute_env(tokens->data[i].content, env);
			if (r.is_error)
				return (false);
			if (r.value[0] == '\0' && unquoted)
			{
				FREE(r.value);
				if (tokens->data[i].allocated)
					FREE(tokens->data[i].content);
				i++;
				continue ;
			}
			if (tokens->data[i].allocated)
				FREE(tokens->data[i].content);
			tokens->data[i].content = r.value;
			tokens->data[i].allocated = true;
		}
		tokens->data[j] = tokens->data[i];
		j++;
		i++;
	}
	tokens->size = j;
	return (true);
}
