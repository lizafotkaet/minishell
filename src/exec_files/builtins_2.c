/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_2.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:01:22 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/22 19:19:49 by asrichar         ###   ########.fr       */
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
	t_result_t_env_key_value_pair	r;

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

int	builtin_export(t_cmd *cmd, t_env *env)
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

int	builtin_unset(t_cmd *cmd, t_env *env)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
		m_env_remove(env, cmd->argv[i++]);
	return (0);
}
