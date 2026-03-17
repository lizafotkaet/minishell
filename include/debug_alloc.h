#ifndef DEBUG_ALLOC_H
# define DEBUG_ALLOC_H

/*
** Debug wrappers for ALLOC / FREE.
**
** This header MUST be included after libft.h so that ft_calloc
** is already declared when the static-inline bodies are compiled.
*/

# include <stddef.h>
# include "libft.h"

# ifdef DEBUG
#  include <stdio.h>

extern int	g_alloc_counter;
extern int	g_fail_on_alloc;
extern int	g_debug_alloc_verbose;

static inline void	*debug_calloc(size_t count, size_t size,
		const char *type, const char *file, int line)
{
	void	*ptr;

	g_alloc_counter++;
	if (g_fail_on_alloc > 0 && g_alloc_counter >= g_fail_on_alloc)
		return (NULL);
	ptr = ft_calloc(count, size);
	if (g_debug_alloc_verbose)
		fprintf(stderr, "ALLOC #%d: %zu * %zu (%s) %s:%d -> %p\n",
			g_alloc_counter, count, size, type, file, line, ptr);
	return (ptr);
}

static inline void	debug_free(void *ptr, const char *file, int line)
{
	if (g_debug_alloc_verbose)
		fprintf(stderr, "FREE:  %p at %s:%d\n", ptr, file, line);
	free(ptr);
}

#  define ALLOC(type, count) \
	((type *)debug_calloc((count), sizeof(type), #type, __FILE__, __LINE__))
#  define FREE(ptr) \
	debug_free((ptr), __FILE__, __LINE__)
#  define SET_ALLOC_FAIL(n) \
	do { g_alloc_counter = 0; g_fail_on_alloc = (n); } while (0)
#  define RESET_ALLOC_FAIL() \
	do { g_alloc_counter = 0; g_fail_on_alloc = 0; } while (0)
# else
#  define ALLOC(type, count) \
	((type *)ft_calloc((count), sizeof(type)))
#  define FREE(ptr) \
	free((ptr))
#  define SET_ALLOC_FAIL(n) ((void)0)
#  define RESET_ALLOC_FAIL() ((void)0)
# endif

#endif
