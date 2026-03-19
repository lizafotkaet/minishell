#include "../../include/substitute_env_internal.h"
#include "../../libft/libft.h"
#include "../../include/debug_alloc.h"

/*
** Returns the length of the variable name starting at `s`.
** A variable name is [a-zA-Z_][a-zA-Z0-9_]*.
*/
size_t	env_var_name_len(const char *s)
{
	size_t	len;

	if (s == NULL || (*s != '_' && !ft_isalpha(*s)))
		return (0);
	len = 1;
	while (s[len] == '_' || ft_isalpha(s[len]) || ft_isdigit(s[len]))
		len++;
	return (len);
}

/*
** Expand $? -- writes the previous command exit code into buf.
*/
static const char	*expand_exit_code(const char *it, t_env env,
		t_buffer *buf)
{
	char	*str;

	str = ft_itoa(env.previous_command_exit_code);
	if (str == NULL)
		return (NULL);
	if (!m_buffer_write(buf, str, ft_strlen(str)))
	{
		FREE(str);
		return (NULL);
	}
	FREE(str);
	return (it + 1);
}

/*
** Expand $VAR at `it` (points at '$').
** Returns pointer past the variable name, or NULL on failure.
*/
const char	*expand_variable(const char *it, t_env env,
		t_buffer *buf)
{
	size_t		name_len;
	char		*name;
	const char	*val;

	it++;
	if (*it == '?')
		return (expand_exit_code(it, env, buf));
	name_len = env_var_name_len(it);
	if (name_len == 0)
	{
		if (!m_buffer_write(buf, "$", 1))
			return (NULL);
		return (it);
	}
	name = ft_substr(it, 0, name_len);
	if (name == NULL)
		return (NULL);
	val = m_env_find_value(env, name);
	if (!m_buffer_write(buf, val, ft_strlen(val)))
	{
		FREE(name);
		return (NULL);
	}
	FREE(name);
	return (it + name_len);
}

/*
** Process one character inside a double-quoted segment:
** expand $VAR, handle backslash escapes for " \\ $,
** or copy the character literally.
** Returns pointer past the consumed character(s), or NULL on failure.
*/
const char	*expand_char_within_double_quotes(const char *it,
		const char *end, t_env env, t_buffer *buf)
{
	if (*it == '\\' && it + 1 < end
		&& (it[1] == '"' || it[1] == '\\' || it[1] == '$'))
	{
		it++;
		if (!m_buffer_write(buf, it, 1))
			return (NULL);
		return (it + 1);
	}
	if (*it == '$')
	{
		it = expand_variable(it, env, buf);
		return (it);
	}
	if (!m_buffer_write(buf, it, 1))
		return (NULL);
	return (it + 1);
}
