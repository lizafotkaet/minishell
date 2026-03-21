/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:01:22 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 22:11:00 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static void	export_one(const char *arg, t_env *env, int *exit_code)
{
	RESULT(t_env_key_value_pair)	r;

	r = parse_env_var(arg);
	if (r.is_error)
		return ;
	if (!is_valid_identifier(r.value.key, strlen(r.value.key)))
	{
		write(2, "export: invalid format\n", 23);
		*exit_code = 1;
		m_env_key_value_pair_free(&r.value);
		return ;
	}
	m_env_append(env, r.value);
}

static void	print_export(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->count)
	{
		printf("declare -x %s=\"%s\"\n",
			env->pairs[i].key, env->pairs[i].value);
		i++;
	}
}

static int	builtin_export(t_cmd *cmd, t_env *env)
{
	int	i;
	int	exit_code;

	if (!cmd->argv[1])
	{
		print_export(env);
		return (0);
	}
	i = 1;
	exit_code = 0;
	while (cmd->argv[i])
		export_one(cmd->argv[i++], env, &exit_code);
	return (exit_code);
}

static int	builtin_unset(t_cmd *cmd, t_env *env)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
		m_env_remove(env, cmd->argv[i++]);
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
// atoi silently truncates on overflow 
// — for correctness use atoll and cast down:
// exit((unsigned char)(long long)atoll(cmd->argv[1]));

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
