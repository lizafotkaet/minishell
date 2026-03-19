#include "../../include/tokenize.h"
#include <stdbool.h>

bool	is_whitespace(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\v' || c == '\f' || c == '\r');
}

const char	*skip_consecutive_whitespaces(const char *s)
{
	while (*s != '\0' && is_whitespace(*s))
		s++;
	return (s);
}
