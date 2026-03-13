#ifndef TESTLIB_H
# define TESTLIB_H

/*
** Header-only test library for minishell.
**
** Provides:
**   ASSERT(expr, fmt, ...)        — assert with printf-style message
**   ASSERT_STR_ARR_EQ(a, b)      — assert two NULL-terminated char** are equal
**   ASSERT_ERROR(res)             — assert that a RESULT is an error
**   ASSERT_NO_ERROR(res)          — assert that a RESULT is not an error
**   RUN_TEST(fn)                  — run a test function, report pass/fail
**   TEST_SUMMARY()                — print totals and return exit code
*/

# include <stdio.h>
# include <stdlib.h>
# include <stdbool.h>
# include <string.h>

/* ── internal counters (define in exactly one TU via TESTLIB_IMPL) ──────── */

# ifndef TESTLIB_IMPL
#  define TESTLIB_STORAGE extern
# else
#  define TESTLIB_STORAGE
# endif

TESTLIB_STORAGE int	g_tests_run;
TESTLIB_STORAGE int	g_tests_failed;

# ifdef TESTLIB_IMPL
int	g_tests_run    = 0;
int	g_tests_failed = 0;
# endif

/* ── ASSERT ─────────────────────────────────────────────────────────────── */
/*
** Evaluates `expr`.  On failure prints file, line, the expression text,
** and a user-supplied printf-style message, then returns from the caller.
**
** Usage:
**   ASSERT(x == 42, "expected 42, got %d", x);
*/
# define ASSERT(expr, fmt, ...) \
	do { \
		if (!(expr)) \
		{ \
			printf("  FAIL %s:%d: %s\n        ", \
				__FILE__, __LINE__, #expr); \
			printf(fmt __VA_OPT__(,) __VA_ARGS__); \
			printf("\n"); \
			g_tests_failed++; \
			return ; \
		} \
	} while (0)

/* ── ASSERT_ERROR / ASSERT_NO_ERROR ─────────────────────────────────────── */
/*
** Work with any RESULT(T) value.  They inspect the `.is_error` field.
**
** Usage:
**   RESULT(int) r = some_call();
**   ASSERT_NO_ERROR(r);          // fail if r.is_error
**   ASSERT_ERROR(bad_call());    // fail if !r.is_error
*/
# define ASSERT_ERROR(res) \
	ASSERT((res).is_error, "expected ERROR, got SUCCESS")

# define ASSERT_NO_ERROR(res) \
	ASSERT(!(res).is_error, "expected SUCCESS, got ERROR")

/* ── str_arr_eq ─────────────────────────────────────────────────────────── */
/*
** Returns 1 if two NULL-terminated string arrays are element-wise equal,
** 0 otherwise.  Two NULL pointers are considered equal.
*/
static inline bool	str_arr_eq(char **a, char **b)
{
	int	i;

	if (a == b)
		return (true);
	if (a == NULL || b == NULL)
		return (false);
	i = 0;
	while (a[i] != NULL && b[i] != NULL)
	{
		if (strcmp(a[i], b[i]) != 0)
			return (false);
		i++;
	}
	return (a[i] == NULL && b[i] == NULL);
}

/* helper: print a char** for diagnostics */
static inline void	print_str_arr(const char *label, char **arr)
{
	int	i;

	printf("        %s: {", label);
	if (arr == NULL)
	{
		printf("NULL}\n");
		return ;
	}
	i = 0;
	while (arr[i] != NULL)
	{
		if (i > 0)
			printf(", ");
		printf("\"%s\"", arr[i]);
		i++;
	}
	printf("}\n");
}

# define ASSERT_STR_ARR_EQ(a, b) \
	do { \
		if (!str_arr_eq((a), (b))) \
		{ \
			printf("  FAIL %s:%d: ASSERT_STR_ARR_EQ(%s, %s)\n", \
				__FILE__, __LINE__, #a, #b); \
			print_str_arr("got     ", (a)); \
			print_str_arr("expected", (b)); \
			g_tests_failed++; \
			return ; \
		} \
	} while (0)

/* ── test runner helpers ────────────────────────────────────────────────── */
/*
** RUN_TEST(fn) — call `fn()`, count it, report pass/fail.
** TEST_SUMMARY() — print totals, return 0 on all-pass, 1 on any failure.
*/
# define RUN_TEST(fn) \
	do { \
		int _before = g_tests_failed; \
		g_tests_run++; \
		fn(); \
		if (g_tests_failed == _before) \
			printf("  OK   %s\n", #fn); \
	} while (0)

# define TEST_SUMMARY() \
	( \
		printf("\n%d/%d tests passed\n", \
			g_tests_run - g_tests_failed, g_tests_run), \
		(g_tests_failed != 0) \
	)

#endif
