#include "quotes.h"
#include <stddef.h>

/*
** Counts consecutive backslashes going backwards from (pos - 1).
** `start` is the beginning of the string so we don't read before it.
** Returns the number of consecutive '\' characters immediately before `pos`.
*/
static int	count_preceding_backslashes(const char *start, const char *pos)
{
	int	count;

	count = 0;
	pos--;
	while (pos >= start && *pos == '\\')
	{
		count++;
		pos--;
	}
	return (count);
}

/*
** Returns true when the character at `pos` is escaped by a preceding
** backslash (i.e. there is an odd number of consecutive backslashes
** right before it).
**
** `start` is the beginning of the string (used as a boundary).
*/
bool	character_is_escaped(const char *start, const char *pos)
{
	if (pos <= start)
		return (false);
	return (count_preceding_backslashes(start, pos) % 2 != 0);
}

/*
** Finds the closing double-quote that matches the opening one at
** `quote_position`.  Backslash-escaped quotes (\") are skipped.
**
** Returns SUCCESS with a pointer to the closing '"', or ERROR if
** the closing quote is never found (unterminated string).
*/
RESULT(t_const_char_ptr)	closing_double_quote_position(
		const char *quote_position)
{
	const char	*it;

	it = quote_position + 1;
	while (*it != '\0')
	{
		if (*it == '"' && !character_is_escaped(quote_position, it))
			return (SUCCESS(t_const_char_ptr, it));
		it++;
	}
	return (ERROR(t_const_char_ptr));
}

/*
** Finds the closing single-quote that matches the opening one at
** `quote_position`.  In single-quoted strings nothing is escaped --
** the next single-quote always closes the string.
**
** Returns SUCCESS with a pointer to the closing '\'', or ERROR if
** the closing quote is never found.
*/
RESULT(t_const_char_ptr)	closing_single_quote_position(
		const char *quote_position)
{
	const char	*it;

	it = quote_position + 1;
	while (*it != '\0')
	{
		if (*it == '\'')
			return (SUCCESS(t_const_char_ptr, it));
		it++;
	}
	return (ERROR(t_const_char_ptr));
}
