#ifndef RESULT_H
# define RESULT_H

# include <stdbool.h>

/*
** RESULT macro -- parametric result type with error indication.
**
** Defines a struct containing:
**   - bool    is_error : true on failure, false on success
**   - T       value    : the wrapped return value
**
** Usage example:
**
**   RESULT(int)  divide(int a, int b)
**   {
**       if (b == 0)
**           return (ERROR(int));
**       return (SUCCESS(int, a / b));
**   }
**
**   // caller:
**   t_result_int r = divide(10, 2);
**   if (r.is_error)
**       handle_error();
**   else
**       use(r.value);
*/

# define RESULT(T) \
	t_result_##T

# define DECLARE_RESULT(T) \
	typedef struct s_result_##T \
	{ \
		bool	is_error; \
		T		value; \
	}	t_result_##T

# define ERROR(T) \
	((t_result_##T){.is_error = true})

# define SUCCESS(T, val) \
	((t_result_##T){.is_error = false, .value = (val)})

/*
** RETURN_ON_ERROR -- propagate an error from an inner result.
**
** `T`   : the result type of the *enclosing* function (return type tag).
** `res` : an already-evaluated result expression of any RESULT type.
**
** If `res.is_error` is true the macro makes the enclosing function
** return ERROR(T) immediately.
**
** Usage:
**   RESULT(int) outer(void)
**   {
**       RESULT(t_const_char_ptr) r = some_call();
**       RETURN_ON_ERROR(int, r);
**       // … use r.value …
**   }
*/
# define RETURN_ON_ERROR(T, res) \
	do { /* do-while(0): ensures the macro acts as a single statement */ \
		if ((res).is_error) \
			return (ERROR(T)); \
	} while (0)

/* Common result type for char * -- used across multiple modules. */
typedef char	*t_char_ptr;
DECLARE_RESULT(t_char_ptr);

#endif
