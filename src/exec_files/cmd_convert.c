/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_convert.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:13:04 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 17:29:37 by asrichar         ###   ########.fr       */
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

static void	fill_redirs(t_cmd *node, t_command *cmd)
{
	int	j;
	int	k;

	j = 0;
	k = 0;
	while (j < cmd->redirect_count)
	{
		if (cmd->redirects[j].type == E_HEREDOC)
		{
			if (cmd->heredoc_contents.size > 0)
				node->heredoc_tmpfile
					= (char *)cmd->heredoc_contents.data[0];
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
