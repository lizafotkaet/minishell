#include "minishell.h"

static void	run_child_process(t_cmd *cur, int i, int n_cmds,
				int **pipes, char **envp)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	apply_all_redirs(cur, i, n_cmds, pipes);
	close_all_pipes(pipes, n_cmds);
	if (!cur->argv[0] || !cur->argv[0][0])
		exit(0);
	if (is_builtin(cur->argv[0]))
		exit(execute_builtin(cur, envp));
	else
		exec_with_path(cur->argv[0], cur->argv, envp);
}

static pid_t	fork_and_exec(t_cmd *current, int i, int n_cmds,
				int **pipes, char **envp)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		run_child_process(current, i, n_cmds, pipes, envp);
	return (pid);
}

pid_t	exec_one_cmd(t_cmd *cmd, int i, int n_cmds,
			int **pipes, char **envp)
{
	if (!cmd->argv || !cmd->argv[0])
		return (0);
	if (n_cmds == 1 && is_builtin(cmd->argv[0]))
		return (-2);
	return (fork_and_exec(cmd, i, n_cmds, pipes, envp));
}