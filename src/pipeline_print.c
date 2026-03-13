#include "pipeline_print.h"
#include "command_print.h"
#include "../libft/libft.h"

static bool	buf_str(t_buffer *buf, const char *s)
{
	if (s == NULL)
		return (true);
	return (m_buffer_write(buf, s, ft_strlen(s)));
}

RESULT(t_char_ptr)	pipeline_to_xml(t_pipeline *pl)
{
	RESULT(t_buffer)	br;
	t_buffer			buf;
	int					i;
	char				*result;

	br = m_buffer_new();
	if (br.is_error)
		return (ERROR(t_char_ptr));
	buf = br.value;
	if (!buf_str(&buf, "<pipeline>\n"))
		return (m_buffer_free(&buf), ERROR(t_char_ptr));
	i = 0;
	while (i < pl->command_count)
	{
		if (!write_command_xml(&buf, &pl->commands[i]))
			return (m_buffer_free(&buf), ERROR(t_char_ptr));
		i++;
	}
	if (!buf_str(&buf, "</pipeline>\n"))
		return (m_buffer_free(&buf), ERROR(t_char_ptr));
	result = m_buffer_read(&buf);
	m_buffer_free(&buf);
	if (!result)
		return (ERROR(t_char_ptr));
	return (SUCCESS(t_char_ptr, result));
}
