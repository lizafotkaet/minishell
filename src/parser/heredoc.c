/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:39 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 19:16:22 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipeline.h"
#include "buffer.h"
#include "libft.h"
#include "debug_alloc.h"
#include <unistd.h>

static bool	line_is_delim(const char *line, size_t len, const char *delim)
{
	size_t	dlen;

	if (len > 0 && line[len - 1] == '\n')
		len--;
	dlen = ft_strlen(delim);
	return (len == dlen && ft_strncmp(line, delim, dlen) == 0);
}

static ssize_t	read_line_to_buf(t_buffer *line, int is_tty)
{
	char	page[4096];
	ssize_t	n;
	size_t	i;

	line->size = 0;
	if (line->data)
		line->data[0] = '\0';
	if (is_tty)
		write(STDERR_FILENO, "> ", 2);
	n = read(STDIN_FILENO, page, sizeof(page) - 1);
	if (n <= 0)
		return (n);
	page[n] = '\0';
	i = 0;
	while (i < (size_t)n)
	{
		if (!m_buffer_write(line, &page[i], 1))
			return (-1);
		if (page[i] == '\n')
			break ;
		i++;
	}
	return ((ssize_t)line->size);
}

static char	*read_one_heredoc(const char *delim, int is_tty)
{
	t_result_t_buffer	content_r;
	t_result_t_buffer	line_r;
	t_buffer			content;
	t_buffer			line;
	char				*result;

	content_r = m_buffer_new();
	if (content_r.is_error)
		return (NULL);
	content = content_r.value;
	line_r = m_buffer_new();
	if (line_r.is_error)
		return (m_buffer_free(&content), NULL);
	line = line_r.value;
	while (read_line_to_buf(&line, is_tty) > 0
		&& !line_is_delim(line.data, line.size, delim))
	{
		if (!m_buffer_write(&content, line.data, line.size))
			return (m_buffer_free(&line), m_buffer_free(&content), NULL);
	}
	m_buffer_free(&line);
	result = m_buffer_read(&content);
	m_buffer_free(&content);
	return (result);
}

// the function that goes through one command, 
// checks if it has heredocs and goes through all of them

static bool	read_cmd_heredocs(t_command *cmd, int is_tty)
{
	t_result_t_string_vector	vr;
	int							i;
	char						*content;

	vr = m_string_vector_new();
	if (vr.is_error)
		return (false);
	cmd->heredoc_contents = vr.value;
	i = 0;
	while (i < cmd->redirect_count)
	{
		if (cmd->redirects[i].type == E_HEREDOC)
		{
			content = read_one_heredoc(cmd->redirects[i].target, is_tty);
			if (!content)
				return (false);
			if (!m_string_vector_append(&cmd->heredoc_contents, content))
				return (free(content), false);
			free(content);
		}
		i++;
	}
	return (true);
}

// goes through all the commands in the pipes and looks for heredocs

bool	read_heredocs(t_pipeline *pl)
{
	int	i;
	int	is_tty;

	if (pl == NULL)
		return (false);
	is_tty = isatty(STDIN_FILENO);
	i = 0;
	while (i < pl->command_count)
	{
		if (!read_cmd_heredocs(&pl->commands[i], is_tty))
			return (false);
		i++;
	}
	return (true);
}
