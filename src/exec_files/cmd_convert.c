/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_convert.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:13:04 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 20:25:57 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"
#include "pipeline.h"

static int	count_redirs(t_command *cmd)
{
	int	n;
	int	j;

	n = 0;
	j = 0;
	while (j < cmd->redirect_count)
	{
		if (cmd->redirects[j].type != E_HEREDOC)
			n++;
		j++;
	}
	return (n);
}

static void	fill_redir(t_redir_exec *r, t_redirect *red)
{
	if (red->type == E_REDIR_IN)
	{
		r->file = red->target;
		r->type = REDIR_IN;
	}
	else if (red->type == E_REDIR_OUT)
	{
		r->file = red->target;
		r->type = REDIR_OUT;
	}
	else if (red->type == E_REDIR_APPEND)
	{
		r->file = red->target;
		r->type = REDIR_APPEND;
	}
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

static void	fill_redirs(t_cmd *node, t_command *cmd)
{
	int	j;
	int	k;
	t_string_vector contents;
	j = 0;
	k = 0;
	while (j < cmd->redirect_count)
	{
		if (cmd->redirects[j].type == E_HEREDOC)
		{
			if (cmd->heredoc_contents.size > 0)
			{
				contents = cmd->heredoc_contents;
			}
				node->heredoc_tmpfile
					= heredoc_to_tmpfile((char *)contents.data[contents.size - 1], j, 0);
		}
		else
			fill_redir(&node->redirs[k++], &cmd->redirects[j]);
		j++;
	}
}

static t_cmd	*alloc_node(t_command *cmd, t_cmd *head)
{
	t_cmd	*node;

	node = (t_cmd *)calloc(1, sizeof(t_cmd));
	if (!node)
		return (free_cmd_list(head), NULL);
	node->argv = cmd->argv;
	node->n_redirs = count_redirs(cmd);
	if (node->n_redirs == 0)
		return (node);
	node->redirs = malloc(sizeof(t_redir_exec) * node->n_redirs);
	if (!node->redirs)
		return (free(node), free_cmd_list(head), NULL);
	return (node);
}

t_cmd	*pipeline_to_cmd_list(t_pipeline *pl)
{
	t_cmd	*head;
	t_cmd	*node;
	int		i;

	head = NULL;
	i = pl->command_count - 1;
	while (i >= 0)
	{
		node = alloc_node(&pl->commands[i], head);
		if (!node)
			return (NULL);
		fill_redirs(node, &pl->commands[i]);
		node->next = head;
		head = node;
		i--;
	}
	return (head);
}
