#include "../../include/env.h"
#include "../../include/quotes.h"
#include "../../include/buffer.h"
#include "../../include/substitute_env_internal.h"
#include "../../libft/libft.h"

/*
** Process a single-quoted segment: copy everything literally
** until closing quote.  Returns pointer past the closing quote,
** or ERROR on unterminated quote.
*/
static RESULT(t_const_char_ptr)	process_single_quoted(const char *it,
		t_buffer *buf)
{
	RESULT(t_const_char_ptr)	close;

	close = closing_single_quote_position(it);
	if (close.is_error)
		return (ERROR(t_const_char_ptr));
	it++;
	while (it < close.value)
	{
		if (!m_buffer_write(buf, it, 1))
			return (ERROR(t_const_char_ptr));
		it++;
	}
	return (SUCCESS(t_const_char_ptr, close.value + 1));
}

/*
** Process a double-quoted segment: expand $VAR, handle \" \\ \$,
** copy everything else literally.  Uses closing_double_quote_position
** to find the end, then processes content up to that point.
** Returns pointer past the closing quote, or ERROR on unterminated.
*/
static RESULT(t_const_char_ptr)	process_double_quoted(const char *it,
		t_env env, t_buffer *buf)
{
	RESULT(t_const_char_ptr)	close;
	const char					*end;

	close = closing_double_quote_position(it);
	if (close.is_error)
		return (ERROR(t_const_char_ptr));
	end = close.value;
	it++;
	while (it < end)
	{
		it = expand_char_within_double_quotes(it, end, env, buf);
		if (it == NULL)
			return (ERROR(t_const_char_ptr));
	}
	return (SUCCESS(t_const_char_ptr, end + 1));
}

static RESULT(t_const_char_ptr)	process_unquoted_char(const char *it,
		t_env env, t_buffer *buf)
{
	if (*it == '\\' && it[1] != '\0')
	{
		it++;
		if (!m_buffer_write(buf, it, 1))
			return (ERROR(t_const_char_ptr));
		return (SUCCESS(t_const_char_ptr, it + 1));
	}
	if (*it == '$')
	{
		it = expand_variable(it, env, buf);
		if (it == NULL)
			return (ERROR(t_const_char_ptr));
		return (SUCCESS(t_const_char_ptr, it));
	}
	if (!m_buffer_write(buf, it, 1))
		return (ERROR(t_const_char_ptr));
	return (SUCCESS(t_const_char_ptr, it + 1));
}

/*
** Process unquoted text: expand $VAR, handle backslash escapes,
** delegate to quoted processors on quote characters.
** Returns pointer past the processed text, or ERROR on unterminated quote.
*/
static RESULT(t_const_char_ptr)	substitute_env_with_buffer(const char *it,
		t_env env, t_buffer *buf)
{
	RESULT(t_const_char_ptr)	r;

	while (*it != '\0')
	{
		if (*it == '\'')
		{
			r = process_single_quoted(it, buf);
			RETURN_ON_ERROR(t_const_char_ptr, r);
			it = r.value;
		}
		else if (*it == '"')
		{
			r = process_double_quoted(it, env, buf);
			RETURN_ON_ERROR(t_const_char_ptr, r);
			it = r.value;
		}
		else
		{
			r = process_unquoted_char(it, env, buf);
			RETURN_ON_ERROR(t_const_char_ptr, r);
			it = r.value;
		}
	}
	return (SUCCESS(t_const_char_ptr, it));
}

/*
** Substitutes environment variables, removes quotes, and unescapes
** backslash sequences in the raw token string `input`.
**
** Returns RESULT(t_char_ptr): SUCCESS with the new heap string,
** or ERROR on unterminated quote or allocation failure.
*/
RESULT(t_char_ptr)	substitute_env(const char *input, t_env env)
{
	RESULT(t_buffer)			br;
	RESULT(t_const_char_ptr)	r;
	t_buffer					buf;
	char						*result;

	if (input == NULL)
		return (ERROR(t_char_ptr));
	br = m_buffer_new();
	if (br.is_error)
		return (ERROR(t_char_ptr));
	buf = br.value;
	r = substitute_env_with_buffer(input, env, &buf);
	if (r.is_error)
	{
		m_buffer_free(&buf);
		return (ERROR(t_char_ptr));
	}
	result = m_buffer_read(&buf);
	m_buffer_free(&buf);
	if (!result)
		return (ERROR(t_char_ptr));
	return (SUCCESS(t_char_ptr, result));
}
