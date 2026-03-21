/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:11:39 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 17:12:43 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	return (strcmp(cmd, "echo") == 0 || strcmp(cmd, "cd") == 0
		|| strcmp(cmd, "pwd") == 0 || strcmp(cmd, "export") == 0
		|| strcmp(cmd, "unset") == 0 || strcmp(cmd, "env") == 0
		|| strcmp(cmd, "exit") == 0);
}

int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->argv[1] && strcmp(cmd->argv[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	while (cmd->argv[i])
	{
		printf("%s", cmd->argv[i]);
		if (cmd->argv[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

static void	update_pwd_env(t_env *env, const char *new_pwd)
{
	t_env_key_value_pair	pair;
	const char				*old;

	old = m_env_find_value(*env, "PWD");
	pair.key = ft_strdup("OLDPWD");
	pair.value = ft_strdup(old);
	if (pair.key && pair.value)
		m_env_append(env, pair);
	else
		m_env_key_value_pair_free(&pair);
	pair.key = ft_strdup("PWD");
	pair.value = ft_strdup(new_pwd);
	if (pair.key && pair.value)
		m_env_append(env, pair);
	else
		m_env_key_value_pair_free(&pair);
}

int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) == NULL)
	{
		perror("pwd");
		return (1);
	}
	printf("%s\n", cwd);
	return (0);
}

int	builtin_env(t_env *env)
{
	int					i;
	t_result_t_char_ptr	r;

	i = 0;
	while (i < env->count)
	{
		r = m_env_key_value_pair_to_c_str(env->pairs[i]);
		if (!r.is_error)
		{
			printf("%s\n", r.value);
			FREE(r.value);
		}
		i++;
	}
	return (0);
}

int	builtin_cd(t_cmd *cmd, t_env *env)
{
	char	cwd[1024];

	if (!cmd->argv[1] || cmd->argv[2])
	{
		write(2, "cd: wrong number of arguments\n", 30);
		return (1);
	}
	if (chdir(cmd->argv[1]) == -1)
	{
		perror("cd");
		return (1);
	}
	if (getcwd(cwd, sizeof(cwd)) != NULL)
		update_pwd_env(env, cwd);
	return (0);
}
