/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:14:50 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/23 17:28:37 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	cleanup_heredocs(t_cmd *cmd_list)
{
	t_cmd	*current;

	current = cmd_list;
	while (current)
	{
		if (current->heredoc_tmpfile)
			unlink(current->heredoc_tmpfile);
		current = current->next;
	}
}

static int	wait_all_children(pid_t last_pid, int forked_count)
{
	int	status;
	int	tmp;
	int	i;

	status = 0;
	if (forked_count == 0)
		return (0);
	if (last_pid != -1)
		waitpid(last_pid, &status, 0);
	i = 0;
	while (i < forked_count - 1)
	{
		waitpid(-1, &tmp, 0);
		i++;
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

static int	pipeline_loop(t_cmd *cmd_list, int n_cmds, int **pipes, t_env *env)
{
	t_cmd	*current;
	pid_t	last_pid;
	pid_t	pid;
	int		forked_count;
	int		i;

	i = 0;
	forked_count = 0;
	last_pid = -1;
	current = cmd_list;
	while (current)
	{
		pid = exec_one_cmd(current, &(t_exec_ctx){i, n_cmds, pipes}, env);
		if (pid == -2)
			return (handle_parent_builtin(current, env));
		if (pid == -1)
			return (1);
		forked_count += (pid > 0);
		if (pid > 0)
			last_pid = pid;
		current = current->next;
		i++;
	}
	close_parent_pipes(pipes, n_cmds - 1);
	return (wait_all_children(last_pid, forked_count));
}

int	run_pipeline(t_cmd *cmd_list, t_env *env)
{
	int		n_cmds;
	int		**pipes;
	int		status;

	n_cmds = count_cmds(cmd_list);
	pipes = alloc_pipes(n_cmds);
	if (n_cmds > 1 && !pipes)
		return (1);
	status = pipeline_loop(cmd_list, n_cmds, pipes, env);
	free_pipes(pipes, n_cmds);
	cleanup_heredocs(cmd_list);
	g_signal = 0;
	return (status);
}
