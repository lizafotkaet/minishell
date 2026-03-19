
#include <ctype.h>
#include "minishell.h"

static int	is_valid_identifier(const char *s, size_t len)
{
	size_t	i;

	if (len == 0 || (s[0] != '_' && !isalpha((unsigned char)s[0])))
		return (0);
	i = 1;
	while (i < len)
	{
		if (s[i] != '_' && !isalpha((unsigned char)s[i])
			&& !isdigit((unsigned char)s[i]))
			return (0);
		i++;
	}
	return (1);
}

static int	builtin_export(t_cmd *cmd)
{
	int		i;
	int		exit_code;
	char	*eq;
	size_t	name_len;

	i = 1;
	exit_code = 0;
	while (cmd->argv[i])
	{
		eq = strchr(cmd->argv[i], '=');
		if (eq)
			name_len = (size_t)(eq - cmd->argv[i]);
		else
			name_len = strlen(cmd->argv[i]);
		if (!is_valid_identifier(cmd->argv[i], name_len))
		{
			write(2, "export: invalid format\n", 23);
			exit_code = 1;
		}
		i++;
	}
	return (exit_code);
}

static int	builtin_unset(t_cmd *cmd)
{
	(void)cmd;
	return (0);
}

static int	is_numeric(const char *s)
{
	if (*s == '+' || *s == '-')
		s++;
	if (*s == '\0')
		return (0);
	while (*s)
	{
		if (*s < '0' || *s > '9')
			return (0);
		s++;
	}
	return (1);
}

static int	builtin_exit(t_cmd *cmd)
{
	if (cmd->argv[1] && cmd->argv[2])
	{
		write(2, "exit: too many arguments\n", 25);
		return (1);
	}
	if (cmd->argv[1] && !is_numeric(cmd->argv[1]))
	{
		write(2, "exit: numeric argument required\n", 32);
		exit(2);
	}
	if (cmd->argv[1])
		exit((unsigned char)atoi(cmd->argv[1]));
	exit(0);
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