#ifndef QUOTES_H
# define QUOTES_H

# include <stdbool.h>
# include "result.h"

/* Declare result type for char pointers (const char *) */
typedef const char	*t_const_char_ptr;
DECLARE_RESULT(t_const_char_ptr);

bool				character_is_escaped(const char *start, const char *pos);
RESULT(t_const_char_ptr)	closing_double_quote_position(
					const char *quote_position);
RESULT(t_const_char_ptr)	closing_single_quote_position(
					const char *quote_position);

#endif
