#define TESTLIB_IMPL
#include "testlib.h"
#include "quotes.h"
#include <string.h>

/* ── single-quote tests ─────────────────────────────────────────────────── */

void	test_unclosed_single_quote(void)
{
	RESULT(t_const_char_ptr) r = closing_single_quote_position("'hello");
	ASSERT_ERROR(r);
}

/* ── double-quote tests ─────────────────────────────────────────────────── */

void	test_unclosed_double_quote(void)
{
	RESULT(t_const_char_ptr) r = closing_double_quote_position("\"hello");
	ASSERT_ERROR(r);
}

/*   \"   → 1 backslash escapes the quote → still unclosed */
void	test_unclosed_double_quote_escaped(void)
{
	RESULT(t_const_char_ptr) r = closing_double_quote_position("\"hello\\\"");
	ASSERT_ERROR(r);
}

/* ── closed double-quote, pointer lands on closing '"' ──────────────────── */

/*   "hello"AAA  →  result points at closing '"', +1 is "AAA"  */
void	test_closed_double_quote(void)
{
	const char				*s = "\"hello\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/*   "hel\"lo"AAA  →  \" is escaped, second unescaped '"' closes  */
void	test_closed_double_quote_with_escaped(void)
{
	const char				*s = "\"hel\\\"lo\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/*   "hello\\"AAA  →  \\ is even, so '"' is NOT escaped → closes  */
void	test_closed_double_quote_even_backslashes(void)
{
	const char				*s = "\"hello\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/* ── odd backslash counts (3, 5, 7) → quote IS escaped ─────────────────── */

/*   "hello\\\"   →  3 backslashes (odd) → escaped → unclosed  */
void	test_odd_3_backslashes_escaped(void)
{
	const char				*s = "\"hello\\\\\\\"";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_ERROR(r);
}

/*   "hello\\\\\"   →  5 backslashes (odd) → escaped → unclosed  */
void	test_odd_5_backslashes_escaped(void)
{
	const char				*s = "\"hello\\\\\\\\\\\"";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_ERROR(r);
}

/*   "hello\\\\\\\"   →  7 backslashes (odd) → escaped → unclosed  */
void	test_odd_7_backslashes_escaped(void)
{
	const char				*s = "\"hello\\\\\\\\\\\\\\\"";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_ERROR(r);
}

/* ── even backslash counts (4, 6, 8) → quote is NOT escaped → closes ───── */

/*   "hello\\\\"AAA  →  4 backslashes (even) → closes  */
void	test_even_4_backslashes_closed(void)
{
	const char				*s = "\"hello\\\\\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/*   "hello\\\\\\"AAA  →  6 backslashes (even) → closes  */
void	test_even_6_backslashes_closed(void)
{
	const char				*s = "\"hello\\\\\\\\\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/*   "hello\\\\\\\\"AAA  →  8 backslashes (even) → closes  */
void	test_even_8_backslashes_closed(void)
{
	const char				*s = "\"hello\\\\\\\\\\\\\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/* ── with escaped quote inside + odd/even trailing backslashes ──────────── */

/*   "hel\"lo\\\"   →  inner \" + trailing 3 bs (odd) → unclosed  */
void	test_escaped_inside_odd_3(void)
{
	const char				*s = "\"hel\\\"lo\\\\\\\"";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_ERROR(r);
}

/*   "hel\"lo\\\\\"   →  inner \" + trailing 5 bs (odd) → unclosed  */
void	test_escaped_inside_odd_5(void)
{
	const char				*s = "\"hel\\\"lo\\\\\\\\\\\"";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_ERROR(r);
}

/*   "hel\"lo\\\\\\\"   →  inner \" + trailing 7 bs (odd) → unclosed  */
void	test_escaped_inside_odd_7(void)
{
	const char				*s = "\"hel\\\"lo\\\\\\\\\\\\\\\"";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_ERROR(r);
}

/*   "hel\"lo\\\\"AAA  →  inner \" + trailing 4 bs (even) → closes  */
void	test_escaped_inside_even_4(void)
{
	const char				*s = "\"hel\\\"lo\\\\\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/*   "hel\"lo\\\\\\"AAA  →  inner \" + trailing 6 bs (even) → closes  */
void	test_escaped_inside_even_6(void)
{
	const char				*s = "\"hel\\\"lo\\\\\\\\\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/*   "hel\"lo\\\\\\\\"AAA  →  inner \" + trailing 8 bs (even) → closes  */
void	test_escaped_inside_even_8(void)
{
	const char				*s = "\"hel\\\"lo\\\\\\\\\\\\\\\\\"AAA";
	RESULT(t_const_char_ptr)	r = closing_double_quote_position(s);

	ASSERT_NO_ERROR(r);
	ASSERT(strcmp(r.value + 1, "AAA") == 0,
		"expected \"AAA\", got \"%s\"", r.value + 1);
}

/* ── main ───────────────────────────────────────────────────────────────── */

int	main(void)
{
	RUN_TEST(test_unclosed_single_quote);
	RUN_TEST(test_unclosed_double_quote);
	RUN_TEST(test_unclosed_double_quote_escaped);
	RUN_TEST(test_closed_double_quote);
	RUN_TEST(test_closed_double_quote_with_escaped);
	RUN_TEST(test_closed_double_quote_even_backslashes);
	RUN_TEST(test_odd_3_backslashes_escaped);
	RUN_TEST(test_odd_5_backslashes_escaped);
	RUN_TEST(test_odd_7_backslashes_escaped);
	RUN_TEST(test_even_4_backslashes_closed);
	RUN_TEST(test_even_6_backslashes_closed);
	RUN_TEST(test_even_8_backslashes_closed);
	RUN_TEST(test_escaped_inside_odd_3);
	RUN_TEST(test_escaped_inside_odd_5);
	RUN_TEST(test_escaped_inside_odd_7);
	RUN_TEST(test_escaped_inside_even_4);
	RUN_TEST(test_escaped_inside_even_6);
	RUN_TEST(test_escaped_inside_even_8);
	return (TEST_SUMMARY());
}
