/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:15:23 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/22 18:40:30 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	apply_heredoc_child(t_cmd *cmd, int *has_in)
{
	int	fd;

	if (!cmd->heredoc_tmpfile)
		return ;
	fd = open(cmd->heredoc_tmpfile, O_RDONLY);
	if (fd == -1)
	{
		perror(cmd->heredoc_tmpfile);
		exit(1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	*has_in = 1;
}

static void	apply_out_redir_child(t_cmd *cmd, int j, int *has_out)
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
		exit(1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	*has_out = 1;
}

static void	apply_one_redir_child(t_cmd *cmd, int j, int *has_in, int *has_out)
{
	int	fd;

	if (cmd->redirs[j].type == REDIR_IN)
	{
		fd = open(cmd->redirs[j].file, O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->redirs[j].file);
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
		*has_in = 1;
	}
	else
		apply_out_redir_child(cmd, j, has_out);
}

void	apply_all_redirs(t_cmd *current, int i, int n_cmds, int **pipes)
{
	int	j;
	int	has_in;
	int	has_out;

	has_in = 0;
	has_out = 0;
	apply_heredoc_child(current, &has_in);
	j = 0;
	while (j < current->n_redirs)
	{
		apply_one_redir_child(current, j, &has_in, &has_out);
		j++;
	}
	if (!has_in && i > 0 && pipes)
		dup2(pipes[i - 1][0], STDIN_FILENO);
	if (!has_out && i < n_cmds - 1 && pipes)
		dup2(pipes[i][1], STDOUT_FILENO);
}

void	close_all_pipes(int **pipes, int n_cmds)
{
	int	j;

	if (!pipes)
		return ;
	j = 0;
	while (j < n_cmds - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
}
