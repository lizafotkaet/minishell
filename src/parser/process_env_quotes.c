/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_env_quotes.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 19:58:19 by ebarbash          #+#    #+#             */
/*   Updated: 2026/03/22 20:02:06 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "env.h"
#include "quotes.h"
#include "buffer.h"
#include "substitute_env_internal.h"
#include "libft.h"

/*
** Process a single-quoted segment: copy everything literally
** until closing quote.  Returns pointer past the closing quote,
** or ERROR on unterminated quote.
*/

t_result_t_const_char_ptr	process_single_quoted(const char *it,
		t_buffer *buf)
{
	t_result_t_const_char_ptr	close;

	close = closing_single_quote_position(it);
	if (close.is_error)
		return (((t_result_t_const_char_ptr){.is_error = true}));
	it++;
	while (it < close.value)
	{
		if (!m_buffer_write(buf, it, 1))
			return (((t_result_t_const_char_ptr){.is_error = true}));
		it++;
	}
	return (((t_result_t_const_char_ptr){
			.is_error = false, .value = (close.value + 1)}));
}

/*
** Process a double-quoted segment: expand $VAR, handle \" \\ \$,
** copy everything else literally.  Uses closing_double_quote_position
** to find the end, then processes content up to that point.
** Returns pointer past the closing quote, or ERROR on unterminated.
*/

t_result_t_const_char_ptr	process_double_quoted(const char *it,
		t_env env, t_buffer *buf)
{
	t_result_t_const_char_ptr	close;
	const char					*end;

	close = closing_double_quote_position(it);
	if (close.is_error)
		return (((t_result_t_const_char_ptr){.is_error = true}));
	end = close.value;
	it++;
	while (it < end)
	{
		it = expand_char_within_double_quotes(it, end, env, buf);
		if (it == NULL)
			return (((t_result_t_const_char_ptr){.is_error = true}));
	}
	return (((t_result_t_const_char_ptr){
			.is_error = false, .value = (end + 1)}));
}

t_result_t_const_char_ptr	process_unquoted_char(const char *it,
		t_env env, t_buffer *buf)
{
	if (*it == '\\' && it[1] != '\0')
	{
		it++;
		if (!m_buffer_write(buf, it, 1))
			return (((t_result_t_const_char_ptr){.is_error = true}));
		return (((t_result_t_const_char_ptr){
				.is_error = false, .value = (it + 1)}));
	}
	if (*it == '$')
	{
		it = expand_variable(it, env, buf);
		if (it == NULL)
			return (((t_result_t_const_char_ptr){.is_error = true}));
		return (((t_result_t_const_char_ptr){
				.is_error = false, .value = (it)}));
	}
	if (!m_buffer_write(buf, it, 1))
		return (((t_result_t_const_char_ptr){.is_error = true}));
	return (((t_result_t_const_char_ptr){
			.is_error = false, .value = (it + 1)}));
}

// put ifs in a separate function out of the while loop
// in substitute_env_with_buffer

t_result_t_const_char_ptr	process_next_segment(const char *it,
		t_env env, t_buffer *buf)
{
	if (*it == '\'')
		return (process_single_quoted(it, buf));
	else if (*it == '"')
		return (process_double_quoted(it, env, buf));
	return (process_unquoted_char(it, env, buf));
}
