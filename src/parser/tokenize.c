/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:03:36 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:16:23 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "tokenize.h"
#include "libft.h"

/*
** Returns true if `c` is the start of an operator token (|, <, >).
*/

bool	is_operator_char(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

/*
** Returns the length of the operator starting at `s`.
** "<<" and ">>" are two-character operators; everything else is one.
*/

int	operator_length(const char *s)
{
	if ((*s == '<' && *(s + 1) == '<')
		|| (*s == '>' && *(s + 1) == '>'))
		return (2);
	return (1);
}

/*
** Returns the token type for the operator at `s` with length `len`.
*/

t_token_type	operator_type(const char *s, int len)
{
	if (*s == '|')
		return (E_PIPE);
	if (*s == '<' && len == 2)
		return (E_HEREDOC);
	if (*s == '>' && len == 2)
		return (E_REDIR_APPEND);
	if (*s == '>')
		return (E_REDIR_OUT);
	return (E_REDIR_IN);
}

/*
** Advance past a single- or double-quoted substring starting at `it`.
** Returns SUCCESS with a pointer past the closing quote,
** or ERROR on unterminated quote.
*/

t_result_t_const_char_ptr	skip_quoted_segment(const char *it)
{
	t_result_t_const_char_ptr	close;

	if (*it == '"')
	{
		close = closing_double_quote_position(it);
		if (close.is_error)
			return ((t_result_t_const_char_ptr){.is_error = true});
		return (((t_result_t_const_char_ptr){
				.is_error = false, .value = (close.value + 1)}));
	}
	close = closing_single_quote_position(it);
	if (close.is_error)
		return ((t_result_t_const_char_ptr){.is_error = true});
	return (((t_result_t_const_char_ptr){
			.is_error = false, .value = (close.value + 1)}));
}

/*
** Advance past one "word" token starting at `it`.
*/

t_result_t_const_char_ptr	skip_word(const char *it)
{
	t_result_t_const_char_ptr	seg;

	while (*it != '\0' && !is_whitespace(*it) && !is_operator_char(*it))
	{
		if (*it == '"' || *it == '\'')
		{
			seg = skip_quoted_segment(it);
			if (seg.is_error)
				return ((t_result_t_const_char_ptr){.is_error = true});
			it = seg.value;
		}
		else if (*it == '\\' && *(it + 1) != '\0')
			it += 2;
		else
			it++;
	}
	return (((t_result_t_const_char_ptr){.is_error = false, .value = (it)}));
}
