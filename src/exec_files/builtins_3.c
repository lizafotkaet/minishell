/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_3.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 18:55:47 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/22 19:15:49 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static const char	*cd_get_path(t_cmd *cmd, t_env *env)
{
	const char	*path;

	if (cmd->argv[1] && cmd->argv[2])
	{
		write(2, "cd: too many arguments\n", 23);
		return (NULL);
	}
	if (cmd->argv[1])
		return (cmd->argv[1]);
	path = m_env_find_value(*env, "HOME");
	if (path[0] == '\0')
	{
		write(2, "cd: HOME not set\n", 17);
		return (NULL);
	}
	return (path);
}

int	builtin_cd(t_cmd *cmd, t_env *env)
{
	char		cwd[1024];
	const char	*path;

	path = cd_get_path(cmd, env);
	if (!path)
		return (1);
	if (chdir(path) == -1)
	{
		perror("cd");
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		update_pwd_env(env, cwd);
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

int	execute_builtin(t_cmd *cmd, t_env *env)
{
	if (strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(cmd));
	if (strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd());
	if (strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(env));
	if (strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(cmd, env));
	if (strcmp(cmd->argv[0], "export") == 0)
		return (builtin_export(cmd, env));
	if (strcmp(cmd->argv[0], "unset") == 0)
		return (builtin_unset(cmd, env));
	if (strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(cmd));
	return (1);
}
