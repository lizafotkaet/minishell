/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_builtin.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:15:08 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 17:36:53 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	apply_all_builtin_redirs(t_cmd *cmd)
{
	int	j;
	int	fd;
	int	flags;

	if (cmd->heredoc_tmpfile)
	{
		fd = open(cmd->heredoc_tmpfile, O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->heredoc_tmpfile);
			return (-1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
	}
	j = 0;
	while (j < cmd->n_redirs)
	{
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
		}
		else
		{
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
		}
		j++;
	}
	return (0);
}

int	handle_parent_builtin(t_cmd *cmd, char **envp)
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
	status = execute_builtin(cmd, envp);
	fflush(stdout);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (status);
}
