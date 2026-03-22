/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_parse.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:18 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 19:07:33 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "debug_alloc.h"
#include "libft.h"

/*
** Constructs a NULL-terminated char ** from the env.
** Each entry is "KEY=VALUE". Caller must free every string and the array.
*/

char	**m_env_to_char_ptr(t_env env)
{
	char				**result;
	int					i;
	t_result_t_char_ptr	r;

	result = ((char **)ft_calloc((env.count + 1), sizeof(char *)));
	if (result == NULL)
		return (NULL);
	i = 0;
	while (i < env.count)
	{
		r = m_env_key_value_pair_to_c_str(env.pairs[i]);
		if (r.is_error)
		{
			while (--i >= 0)
				free(result[i]);
			free(result);
			return (NULL);
		}
		result[i] = r.value;
		i++;
	}
	result[i] = NULL;
	return (result);
}

static bool	parse_env_loop(char **envp, t_env *env)
{
	int								n;
	t_result_t_env_key_value_pair	r;

	n = 0;
	while (envp != NULL && envp[n] != NULL)
	{
		r = parse_env_var(envp[n]);
		if (r.is_error)
			return (false);
		if (!m_env_append(env, r.value))
		{
			m_env_key_value_pair_free(&r.value);
			return (false);
		}
		n++;
	}
	return (true);
}

/*
** Parses a NULL-terminated array of "KEY=VALUE" strings (like envp)
** into a t_env.
*/

t_result_t_env	parse_env(char **envp)
{
	int		n;
	t_env	env;

	n = 0;
	while (envp != NULL && envp[n] != NULL)
		n++;
	env.capacity = ENV_INITIAL_CAPACITY;
	if (n > env.capacity)
		env.capacity = n;
	env.pairs = ((t_env_key_value_pair *)ft_calloc((
					env.capacity), sizeof(t_env_key_value_pair)));
	if (env.pairs == NULL)
		return (((t_result_t_env){.is_error = true}));
	env.count = 0;
	env.previous_command_exit_code = 0;
	if (!parse_env_loop(envp, &env))
	{
		free_env(&env);
		return (((t_result_t_env){.is_error = true}));
	}
	return (((t_result_t_env){.is_error = false, .value = (env)}));
}

/*
** Free the t_env and all its key/value pairs.
*/

void	free_env(t_env *env)
{
	int	i;

	if (env == NULL)
		return ;
	i = 0;
	while (i < env->count)
	{
		m_env_key_value_pair_free(&env->pairs[i]);
		i++;
	}
	free(env->pairs);
	env->pairs = NULL;
	env->count = 0;
	env->capacity = 0;
}
