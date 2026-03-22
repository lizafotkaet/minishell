/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_print.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:45 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 19:21:59 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipeline_print.h"
#include "command_print.h"
#include "libft.h"

static bool	buf_str(t_buffer *buf, const char *s)
{
	if (s == NULL)
		return (true);
	return (m_buffer_write(buf, s, ft_strlen(s)));
}

t_result_t_char_ptr	pipeline_to_xml(t_pipeline *pl)
{
	t_result_t_buffer	br;
	t_buffer			buf;
	int					i;
	char				*result;

	br = m_buffer_new();
	if (br.is_error)
		return (((t_result_t_char_ptr){.is_error = true}));
	buf = br.value;
	if (!buf_str(&buf, "<pipeline>\n"))
		return (m_buffer_free(&buf), ((t_result_t_char_ptr){.is_error = true}));
	i = 0;
	while (i < pl->command_count)
	{
		if (!write_command_xml(&buf, &pl->commands[i]))
			return (m_buffer_free(&buf), ((t_result_t_char_ptr){.is_error = true}));
		i++;
	}
	if (!buf_str(&buf, "</pipeline>\n"))
		return (m_buffer_free(&buf), ((t_result_t_char_ptr){.is_error = true}));
	result = m_buffer_read(&buf);
	m_buffer_free(&buf);
	if (!result)
		return (((t_result_t_char_ptr){.is_error = true}));
	return (((t_result_t_char_ptr){.is_error = false, .value = (result)}));
}
