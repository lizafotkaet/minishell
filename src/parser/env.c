/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:25 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 15:22:49 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "debug_alloc.h"
#include "libft.h"

/*
** Set the previous command exit code in the env.
*/

void	m_env_set_exit_code(t_env *env, int code)
{
	if (env == NULL)
		return ;
	env->previous_command_exit_code = code;
}

/*
** Searches the env for an entry whose key matches `key`.
** Returns the corresponding value, or "" if not found.
*/

const char	*m_env_find_value(t_env env, const char *key)
{
	int	i;

	if (env.pairs == NULL || key == NULL)
		return ("");
	i = 0;
	while (i < env.count)
	{
		if (m_env_key_value_pair_key_equals(env.pairs[i], key))
			return (env.pairs[i].value);
		i++;
	}
	return ("");
}

/*
** Removes the first entry matching `key` by swapping with the last
** element and decrementing count. Frees the removed pair.
*/

bool	m_env_remove(t_env *env, const char *key)
{
	int	i;

	if (env == NULL || key == NULL)
		return (false);
	i = 0;
	while (i < env->count)
	{
		if (m_env_key_value_pair_key_equals(env->pairs[i], key))
		{
			m_env_key_value_pair_free(&env->pairs[i]);
			env->count--;
			env->pairs[i] = env->pairs[env->count];
			return (true);
		}
		i++;
	}
	return (false);
}

static bool	env_grow(t_env *env, int required)
{
	int						new_cap;
	t_env_key_value_pair	*new_pairs;

	if (required <= env->capacity)
		return (true);
	new_cap = env->capacity;
	while (new_cap < required)
		new_cap *= 2;
	new_pairs = ((t_env_key_value_pair *)ft_calloc((new_cap), sizeof(t_env_key_value_pair)));
	if (new_pairs == NULL)
		return (false);
	ft_memcpy(new_pairs, env->pairs,
		sizeof(t_env_key_value_pair) * env->count);
	FREE(env->pairs);
	env->pairs = new_pairs;
	env->capacity = new_cap;
	return (true);
}

/*
** Append a key-value pair to the env, growing if needed.
** If the key already exists, the old entry is removed first (last wins).
*/
bool	m_env_append(t_env *env, t_env_key_value_pair pair)
{
	if (env == NULL)
		return (false);
	m_env_remove(env, pair.key);
	if (!env_grow(env, env->count + 1))
		return (false);
	env->pairs[env->count] = pair;
	env->count++;
	return (true);
}
