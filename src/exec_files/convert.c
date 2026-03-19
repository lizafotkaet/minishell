#include "minishell.h"

void	free_cmd_list(t_cmd *list);

static char	**dup_argv(char **src)
{
	int		n;
	int		i;
	char	**dup;

	n = 0;
	while (src[n])
		n++;
	dup = malloc(sizeof(char *) * (n + 1));
	if (!dup)
		return (NULL);
	i = 0;
	while (i < n)
	{
		dup[i] = strdup(src[i]);
		if (!dup[i])
		{
			while (i > 0)
				free(dup[--i]);
			free(dup);
			return (NULL);
		}
		i++;
	}
	dup[n] = NULL;
	return (dup);
}

static char	*heredoc_to_tmpfile(const char *content, int ci, int hi)
{
	char	path[64];
	int		fd;

	snprintf(path, sizeof(path), "/tmp/msh_%d_%d_%d", getpid(), ci, hi);
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
		return (NULL);
	if (content)
		write(fd, content, strlen(content));
	close(fd);
	return (strdup(path));
}

static int	count_non_heredoc_redirs(t_command *cmd)
{
	int	n;
	int	i;

	n = 0;
	i = 0;
	while (i < cmd->redirect_count)
	{
		if (cmd->redirects[i].type != E_HEREDOC)
			n++;
		i++;
	}
	return (n);
}

static void	process_redirects(t_command *cmd, t_cmd *tc, int ci)
{
	int			hi;
	int			i;
	int			k;
	t_redirect	*r;

	hi = 0;
	k = 0;
	i = 0;
	while (i < cmd->redirect_count)
	{
		r = &cmd->redirects[i];
		if (r->type == E_REDIR_IN)
		{
			tc->redirs[k].file = strdup(r->target);
			tc->redirs[k].type = REDIR_IN;
			k++;
		}
		else if (r->type == E_REDIR_OUT)
		{
			tc->redirs[k].file = strdup(r->target);
			tc->redirs[k].type = REDIR_OUT;
			k++;
		}
		else if (r->type == E_REDIR_APPEND)
		{
			tc->redirs[k].file = strdup(r->target);
			tc->redirs[k].type = REDIR_APPEND;
			k++;
		}
		else if (r->type == E_HEREDOC)
		{
			if (tc->heredoc_tmpfile)
			{
				unlink(tc->heredoc_tmpfile);
				free(tc->heredoc_tmpfile);
			}
			tc->heredoc_tmpfile = heredoc_to_tmpfile(
					cmd->heredoc_contents.data[hi], ci, hi);
			hi++;
		}
		i++;
	}
}

static t_cmd	*make_cmd(t_command *cmd, int ci)
{
	t_cmd	*tc;

	tc = malloc(sizeof(t_cmd));
	if (!tc)
		return (NULL);
	tc->argv = dup_argv(cmd->argv);
	tc->heredoc_tmpfile = NULL;
	tc->n_redirs = count_non_heredoc_redirs(cmd);
	tc->redirs = NULL;
	if (tc->n_redirs > 0)
	{
		tc->redirs = malloc(sizeof(t_redir_exec) * tc->n_redirs);
		if (!tc->redirs)
		{
			free(tc->argv);
			free(tc);
			return (NULL);
		}
	}
	tc->is_builtin = 0;
	tc->next = NULL;
	process_redirects(cmd, tc, ci);
	return (tc);
}

t_cmd	*pipeline_to_cmd_list(t_pipeline *pl)
{
	t_cmd	*head;
	t_cmd	*tail;
	t_cmd	*tc;
	int		i;

	head = NULL;
	tail = NULL;
	i = 0;
	while (i < pl->command_count)
	{
		tc = make_cmd(&pl->commands[i], i);
		if (!tc)
		{
			free_cmd_list(head);
			return (NULL);
		}
		if (!head)
			head = tc;
		else
			tail->next = tc;
		tail = tc;
		i++;
	}
	return (head);
}

void	free_cmd_list(t_cmd *list)
{
	t_cmd	*next;
	int		i;

	while (list)
	{
		next = list->next;
		if (list->argv)
		{
			i = 0;
			while (list->argv[i])
				free(list->argv[i++]);
			free(list->argv);
		}
		if (list->redirs)
		{
			i = 0;
			while (i < list->n_redirs)
				free(list->redirs[i++].file);
			free(list->redirs);
		}
		if (list->heredoc_tmpfile)
			unlink(list->heredoc_tmpfile);
		free(list->heredoc_tmpfile);
		free(list);
		list = next;
	}
}
