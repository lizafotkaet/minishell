#include "../minishell.h"

static int	builtin_export(t_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		if (!strchr(cmd->argv[i], '='))
		{
			write(2, "export: invalid format\n", 23);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	builtin_unset(t_cmd *cmd)
{
	if (!cmd->argv[1])
	{
		write(2, "unset: not enough arguments\n", 28);
		return (1);
	}
	return (0);
}

static int	builtin_exit(t_cmd *cmd)
{
	int	exit_code;

	exit_code = 0;
	if (cmd->argv[1])
		exit_code = atoi(cmd->argv[1]);
	exit(exit_code);
	return (0);
}

int	execute_builtin(t_cmd *cmd, char **envp)
{
	if (strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(cmd));
	if (strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd());
	if (strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(envp));
	if (strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(cmd));
	if (strcmp(cmd->argv[0], "export") == 0)
		return (builtin_export(cmd));
	if (strcmp(cmd->argv[0], "unset") == 0)
		return (builtin_unset(cmd));
	if (strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(cmd));
	return (1);
}