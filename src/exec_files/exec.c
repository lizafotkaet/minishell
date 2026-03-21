/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:14:33 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 17:14:38 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	run_child_process(t_cmd *cur, t_exec_ctx *ctx, char **envp)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	apply_all_redirs(cur, ctx->i, ctx->n_cmds, ctx->pipes);
	close_all_pipes(ctx->pipes, ctx->n_cmds);
	if (!cur->argv[0] || !cur->argv[0][0])
		exit(0);
	if (is_builtin(cur->argv[0]))
		exit(execute_builtin(cur, envp));
	else
		exec_with_path(cur->argv[0], cur->argv, envp);
}

static pid_t	fork_and_exec(t_cmd *current, t_exec_ctx *ctx, char **envp)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		run_child_process(current, ctx, envp);
	return (pid);
}

pid_t	exec_one_cmd(t_cmd *cmd, t_exec_ctx *ctx, char **envp)
{
	if (!cmd->argv || !cmd->argv[0])
		return (0);
	if (ctx->n_cmds == 1 && is_builtin(cmd->argv[0]))
		return (-2);
	return (fork_and_exec(cmd, ctx, envp));
}
