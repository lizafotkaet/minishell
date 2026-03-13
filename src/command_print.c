#include "command_print.h"
#include "string_vector.h"
#include "../libft/libft.h"

static bool	buf_str(t_buffer *buf, const char *s)
{
	if (s == NULL)
		return (true);
	return (m_buffer_write(buf, s, ft_strlen(s)));
}

static const char	*redir_kind_str(t_token_type type)
{
	if (type == E_REDIR_IN)
		return ("in");
	if (type == E_REDIR_OUT)
		return ("out");
	if (type == E_REDIR_APPEND)
		return ("append");
	if (type == E_HEREDOC)
		return ("heredoc");
	return ("unknown");
}

static bool	write_args_xml(t_buffer *buf, t_command *cmd)
{
	int	i;

	if (!buf_str(buf, "\t\t<args>\n"))
		return (false);
	i = 0;
	while (i < cmd->argc)
	{
		if (!buf_str(buf, "\t\t\t<arg>")
			|| !buf_str(buf, cmd->argv[i])
			|| !buf_str(buf, "</arg>\n"))
			return (false);
		i++;
	}
	return (buf_str(buf, "\t\t</args>\n"));
}

static bool	write_one_redir_xml(t_buffer *buf, t_redirect *redir,
		const char *heredoc_content)
{
	const char	*kind;

	kind = redir_kind_str(redir->type);
	if (redir->type == E_HEREDOC)
	{
		if (!buf_str(buf, "\t\t\t<redir kind=\"heredoc\" delimiter=\"")
			|| !buf_str(buf, redir->target)
			|| !buf_str(buf, "\">\n")
			|| !buf_str(buf, heredoc_content))
			return (false);
		return (buf_str(buf, "\t\t\t</redir>\n"));
	}
	if (!buf_str(buf, "\t\t\t<redir kind=\"")
		|| !buf_str(buf, kind)
		|| !buf_str(buf, "\" dst=\"")
		|| !buf_str(buf, redir->target)
		|| !buf_str(buf, "\" />\n"))
		return (false);
	return (true);
}

bool	write_command_xml(t_buffer *buf, t_command *cmd)
{
	int			i;
	int			hi;
	const char	*hc;

	if (!buf_str(buf, "\t<command>\n") || !write_args_xml(buf, cmd))
		return (false);
	if (cmd->redirect_count > 0 && !buf_str(buf, "\t\t<redirects>\n"))
		return (false);
	hi = 0;
	i = 0;
	while (i < cmd->redirect_count)
	{
		hc = NULL;
		if (cmd->redirects[i].type == E_HEREDOC)
			hc = m_string_vector_at(&cmd->heredoc_contents, hi++);
		if (!write_one_redir_xml(buf, &cmd->redirects[i], hc))
			return (false);
		i++;
	}
	if (cmd->redirect_count > 0 && !buf_str(buf, "\t\t</redirects>\n"))
		return (false);
	return (buf_str(buf, "\t</command>\n"));
}
