/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_builtin.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:15:08 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/22 19:17:57 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	apply_heredoc(t_cmd *cmd)
{
	int	fd;

	if (!cmd->heredoc_tmpfile)
		return (0);
	fd = open(cmd->heredoc_tmpfile, O_RDONLY);
	if (fd == -1)
	{
		perror(cmd->heredoc_tmpfile);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	apply_out_redir(t_cmd *cmd, int j)
{
	int	flags;
	int	fd;

	if (cmd->redirs[j].type == REDIR_APPEND)
		flags = O_WRONLY | O_CREAT | O_APPEND;
	else
		flags = O_WRONLY | O_CREAT | O_TRUNC;
	fd = open(cmd->redirs[j].file, flags, 0644);
	if (fd == -1)
	{
		perror(cmd->redirs[j].file);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int	apply_one_redir(t_cmd *cmd, int j)
{
	int	fd;

	if (cmd->redirs[j].type == REDIR_IN)
	{
		fd = open(cmd->redirs[j].file, O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->redirs[j].file);
			return (-1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
		return (0);
	}
	return (apply_out_redir(cmd, j));
}

static int	apply_all_builtin_redirs(t_cmd *cmd)
{
	int	j;

	if (apply_heredoc(cmd) == -1)
		return (-1);
	j = 0;
	while (j < cmd->n_redirs)
	{
		if (apply_one_redir(cmd, j) == -1)
			return (-1);
		j++;
	}
	return (0);
}

int	handle_parent_builtin(t_cmd *cmd, t_env *env)
{
	int	saved_in;
	int	saved_out;
	int	status;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (apply_all_builtin_redirs(cmd) == -1)
	{
		dup2(saved_in, STDIN_FILENO);
		dup2(saved_out, STDOUT_FILENO);
		close(saved_in);
		close(saved_out);
		return (1);
	}
	status = execute_builtin(cmd, env);
	fflush(stdout);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (status);
}
