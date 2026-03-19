/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:01:50 by liza              #+#    #+#             */
/*   Updated: 2026/03/19 13:01:53 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "command.h"
#include "libft.h"
#include "debug_alloc.h"

/*
** Returns true if token_type is a redirect type (not pipe, not word).
*/
static bool	is_redirect_type(t_token_type type)
{
	return (type == E_REDIR_IN || type == E_REDIR_OUT
		|| type == E_REDIR_APPEND || type == E_HEREDOC);
}

/*
** Count the number of redirects and words in a segment
** [start, end) of the token vector.
*/
static bool	count_cmd_parts(t_token_vector *tokens, size_t start,
		size_t end, t_command *cmd)
{
	size_t	i;

	cmd->argc = 0;
	cmd->redirect_count = 0;
	i = start;
	while (i < end)
	{
		if (is_redirect_type(tokens->data[i].token_type))
		{
			if (i + 1 >= end
				|| tokens->data[i + 1].token_type != E_WORD)
				return (false);
			cmd->redirect_count++;
			i += 2;
		}
		else if (tokens->data[i].token_type == E_WORD)
		{
			cmd->argc++;
			i++;
		}
		else
			return (false);
	}
	return (cmd->argc >= 1);
}

static bool	fill_one_redirect(t_redirect *redir, t_token *tok)
{
	redir->type = tok->token_type;
	redir->target = ft_strdup(tok[1].content);
	return (redir->target != NULL);
}

/*
** Fill the argv and redirects arrays of a command from
** a token segment [start, end).
*/
static bool	fill_command(t_token_vector *tokens, size_t start,
		size_t end, t_command *cmd)
{
	size_t	i;
	int		ai;
	int		ri;

	ai = 0;
	ri = 0;
	i = start;
	while (i < end)
	{
		if (is_redirect_type(tokens->data[i].token_type))
		{
			if (!fill_one_redirect(&cmd->redirects[ri], &tokens->data[i]))
				return (false);
			ri++;
			i += 2;
			continue ;
		}
		cmd->argv[ai] = ft_strdup(tokens->data[i].content);
		if (!cmd->argv[ai])
			return (false);
		ai++;
		i++;
	}
	cmd->argv[ai] = NULL;
	return (true);
}

/*
** Build a single t_command from a token segment [start, end).
** Returns false on syntax error or allocation failure.
*/
bool	build_command(t_token_vector *tokens, size_t start,
		size_t end, t_command *cmd)
{
	if (start >= end)
		return (false);
	ft_bzero(cmd, sizeof(t_command));
	if (!count_cmd_parts(tokens, start, end, cmd))
		return (false);
	cmd->argv = ALLOC(char *, cmd->argc + 1);
	if (!cmd->argv)
		return (false);
	if (cmd->redirect_count > 0)
	{
		cmd->redirects = ALLOC(t_redirect, cmd->redirect_count);
		if (!cmd->redirects)
		{
			FREE(cmd->argv);
			cmd->argv = NULL;
			return (false);
		}
	}
	if (!fill_command(tokens, start, end, cmd))
		return (false);
	return (true);
}


