#include "env.h"
#include "debug_alloc.h"
#include "../libft/libft.h"

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
		FREE(pair->key);
	if (pair->value != NULL)
		FREE(pair->value);
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
RESULT(t_char_ptr)	m_env_key_value_pair_to_c_str(t_env_key_value_pair pair)
{
	char	*eq;
	char	*result;

	if (pair.key == NULL || pair.value == NULL)
		return (ERROR(t_char_ptr));
	eq = ft_strjoin(pair.key, "=");
	if (eq == NULL)
		return (ERROR(t_char_ptr));
	result = ft_strjoin(eq, pair.value);
	FREE(eq);
	if (result == NULL)
		return (ERROR(t_char_ptr));
	return (SUCCESS(t_char_ptr, result));
}

/*
** Parses a single "KEY=VALUE" string into a t_env_key_value_pair.
*/
RESULT(t_env_key_value_pair)	parse_env_var(const char *str)
{
	t_env_key_value_pair	pair;
	char					*eq;

	pair = m_env_key_value_pair_new();
	if (str == NULL)
		return (ERROR(t_env_key_value_pair));
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
		return (ERROR(t_env_key_value_pair));
	}
	return (SUCCESS(t_env_key_value_pair, pair));
}
