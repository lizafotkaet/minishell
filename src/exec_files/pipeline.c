#include "../minishell.h"


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

static int	pipeline_loop(t_cmd *cmd_list, int n_cmds, int **pipes, char **envp)
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
		pid = exec_one_cmd(current, i, n_cmds, pipes, envp);
		if (pid == -2)
			return (handle_parent_builtin(current, envp));
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

int	run_pipeline(t_cmd *cmd_list, char **envp)
{
	int		n_cmds;
	int		**pipes;
	int		status;

	n_cmds = count_cmds(cmd_list);
	pipes = alloc_pipes(n_cmds);
	if (n_cmds > 1 && !pipes)
		return (1);
	status = pipeline_loop(cmd_list, n_cmds, pipes, envp);
	free_pipes(pipes, n_cmds);
	cleanup_heredocs(cmd_list);
	g_signal = 0;
	return (status);
}