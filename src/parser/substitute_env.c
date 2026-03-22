/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   substitute_env.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:03:18 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:03:02 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Process unquoted text: expand $VAR, handle backslash escapes,
** delegate to quoted processors on quote characters.
** Returns pointer past the processed text, or ERROR on unterminated quote.
*/

t_result_t_const_char_ptr	substitute_env_with_buffer(const char *it,
		t_env env, t_buffer *buf)
{
	t_result_t_const_char_ptr	r;

	while (*it != '\0')
	{
		r = process_next_segment(it, env, buf);
		if (r.is_error)
			return (((t_result_t_const_char_ptr){.is_error = true}));
		it = r.value;
	}
	return (((t_result_t_const_char_ptr){.is_error = false, .value = (it)}));
}

/*
** Substitutes environment variables, removes quotes, and unescapes
** backslash sequences in the raw token string `input`.
**
** Returns RESULT(t_char_ptr): SUCCESS with the new heap string,
** or ERROR on unterminated quote or allocation failure.
*/

t_result_t_char_ptr	substitute_env(const char *input, t_env env)
{
	t_result_t_buffer			br;
	t_result_t_const_char_ptr	r;
	t_buffer					buf;
	char						*result;

	if (input == NULL)
		return (((t_result_t_char_ptr){.is_error = true}));
	br = m_buffer_new();
	if (br.is_error)
		return (((t_result_t_char_ptr){.is_error = true}));
	buf = br.value;
	r = substitute_env_with_buffer(input, env, &buf);
	if (r.is_error)
	{
		m_buffer_free(&buf);
		return (((t_result_t_char_ptr){.is_error = true}));
	}
	result = m_buffer_read(&buf);
	m_buffer_free(&buf);
	if (!result)
		return (((t_result_t_char_ptr){.is_error = true}));
	return (((t_result_t_char_ptr){.is_error = false, .value = (result)}));
}
