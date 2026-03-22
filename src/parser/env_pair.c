/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_pair.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:11 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 19:03:15 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "env.h"
#include "debug_alloc.h"
#include "libft.h"

/*
** Returns a zeroed-out key-value pair.
*/
t_env_key_value_pair	m_env_key_value_pair_new(void)
{
	t_env_key_value_pair	pair;

	pair.key = NULL;
	pair.value = NULL;
	return (pair);
}

/*
** Frees key and value if non-NULL.
*/
void	m_env_key_value_pair_free(t_env_key_value_pair *pair)
{
	if (pair == NULL)
		return ;
	if (pair->key != NULL)
		free(pair->key);
	if (pair->value != NULL)
		free(pair->value);
	pair->key = NULL;
	pair->value = NULL;
}

/*
** Returns true if pair.key equals key (exact match).
*/
bool	m_env_key_value_pair_key_equals(t_env_key_value_pair pair,
		const char *key)
{
	size_t	len;

	if (pair.key == NULL || key == NULL)
		return (false);
	len = ft_strlen(key);
	return (ft_strlen(pair.key) == len
		&& ft_strncmp(pair.key, key, len) == 0);
}

/*
** Constructs a "KEY=VALUE" C string from a key-value pair.
*/
t_result_t_char_ptr	m_env_key_value_pair_to_c_str(t_env_key_value_pair pair)
{
	char	*eq;
	char	*result;

	if (pair.key == NULL || pair.value == NULL)
		return (((t_result_t_char_ptr){.is_error = true}));
	eq = ft_strjoin(pair.key, "=");
	if (eq == NULL)
		return (((t_result_t_char_ptr){.is_error = true}));
	result = ft_strjoin(eq, pair.value);
	free(eq);
	if (result == NULL)
		return (((t_result_t_char_ptr){.is_error = true}));
	return (((t_result_t_char_ptr){.is_error = false, .value = (result)}));
}

/*
** Parses a single "KEY=VALUE" string into a t_env_key_value_pair.
*/
t_result_t_env_key_value_pair	parse_env_var(const char *str)
{
	t_env_key_value_pair	pair;
	char					*eq;

	pair = m_env_key_value_pair_new();
	if (str == NULL)
		return (((t_result_t_env_key_value_pair){.is_error = true}));
	eq = ft_strchr(str, '=');
	if (eq == NULL)
	{
		pair.key = ft_strdup(str);
		pair.value = ft_strdup("");
	}
	else
	{
		pair.key = ft_substr(str, 0, eq - str);
		pair.value = ft_strdup(eq + 1);
	}
	if (pair.key == NULL || pair.value == NULL)
	{
		m_env_key_value_pair_free(&pair);
		return (((t_result_t_env_key_value_pair){.is_error = true}));
	}
	return (((t_result_t_env_key_value_pair){.is_error = false,
			.value = (pair)}));
}
