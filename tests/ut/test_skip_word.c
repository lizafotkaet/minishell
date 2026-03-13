#define TESTLIB_IMPL
#include "testlib.h"
#include "tokenize.h"
#include <string.h>

/* ── helpers ────────────────────────────────────────────────────────────── */

/*
** Convenience: assert skip_word succeeds and the returned pointer
** equals the expected suffix of the same string.
*/
static void	assert_skip_word_ok(const char *input,
		const char *expected_suffix, const char *label)
{
	RESULT(t_const_char_ptr)	r;

	r = skip_word(input);
	ASSERT(!r.is_error, "%s: expected SUCCESS, got ERROR", label);
	ASSERT(strcmp(r.value, expected_suffix) == 0,
		"%s: expected \"%s\", got \"%s\"", label, expected_suffix, r.value);
}

/* ── 1) word stops at unescaped >> operator ─────────────────────────────── */
/*
** Raw input the function sees:
**   'asdasd'"zxczxc"\ \ \\\\''"""\""sdaaxxxss\>>>FINISH
**
** Breakdown:
**   'asdasd'         — single-quoted segment
**   "zxczxc"         — double-quoted segment
**   \ \              — two escaped spaces
**   \\\\             — four literal backslashes (two escaped pairs)
**   ''               — empty single-quoted
**   ""               — empty double-quoted
**   "\""             — double-quoted containing escaped quote
**   sdaaxxxss        — plain text
**   \>               — escaped > (not an operator)
**   >>FINISH         — unquoted >> operator → skip_word STOPS here
*/
void	test_word_before_operator(void)
{
	const char	*s;

	s = "'asdasd'\"zxczxc\"\\ \\ \\\\\\\\''\"\"\"\\\"\"sdaaxxxss\\>>>FINISH";
	assert_skip_word_ok(s, ">>FINISH", "word_before_operator");
}

/* ── 2) word stops at unquoted whitespace ───────────────────────────────── */
/*
** Raw input:
**   ''ssd"\\"\ xx\>\<\  FINISH
**
** Breakdown:
**   ''               — empty single-quoted
**   ssd              — plain text
**   "\\"             — double-quoted containing \\ (even → closes)
**   \                — escaped space (not a word boundary)
**   xx               — plain text
**   \>\<             — escaped > and escaped <
**   \                — escaped space
**   (space)FINISH    — unquoted space → skip_word STOPS here
*/
void	test_word_before_whitespace(void)
{
	const char	*s;

	s = "''ssd\"\\\\\"\\ xx\\>\\<\\  FINISH";
	assert_skip_word_ok(s, " FINISH", "word_before_whitespace");
}

/* ── 3a) word stops at unquoted pipe | ──────────────────────────────────── */
/*
** Raw: abc"def"\'|REST
**   abc      — plain
**   "def"    — double-quoted
**   \'       — escaped single-quote (not opening a quoted segment)
**   |REST    — unquoted pipe → STOP
*/
void	test_word_before_pipe(void)
{
	const char	*s;

	s = "abc\"def\"\\'|REST";
	assert_skip_word_ok(s, "|REST", "word_before_pipe");
}

/* ── 3b) word stops at unquoted < redirect ──────────────────────────────── */
/*
** Raw: one'two'\><AFTER
**   one      — plain
**   'two'    — single-quoted
**   \>       — escaped > (not operator)
**   <AFTER   — unquoted < → STOP
*/
void	test_word_before_redir_in(void)
{
	const char	*s;

	s = "one'two'\\><AFTER";
	assert_skip_word_ok(s, "<AFTER", "word_before_redir_in");
}

/* ── 3c) word stops at unquoted > redirect ──────────────────────────────── */
/*
** Raw: aaa\<bbb>OUT
**   aaa      — plain
**   \<       — escaped <
**   bbb      — plain
**   >OUT     — unquoted > → STOP
*/
void	test_word_before_redir_out(void)
{
	const char	*s;

	s = "aaa\\<bbb>OUT";
	assert_skip_word_ok(s, ">OUT", "word_before_redir_out");
}

/* ── 3d) word stops at unquoted << heredoc ──────────────────────────────── */
/*
** Raw: foo"bar"<<HERE
**   foo      — plain
**   "bar"    — double-quoted
**   <<HERE   — unquoted << → STOP
*/
void	test_word_before_heredoc(void)
{
	const char	*s;

	s = "foo\"bar\"<<HERE";
	assert_skip_word_ok(s, "<<HERE", "word_before_heredoc");
}

/* ── 4a) unclosed double quote → error ──────────────────────────────────── */

void	test_unclosed_double_quote(void)
{
	RESULT(t_const_char_ptr)	r;

	r = skip_word("hello\"world");
	ASSERT_ERROR(r);
}

/* ── 4b) unclosed single quote → error ──────────────────────────────────── */

void	test_unclosed_single_quote(void)
{
	RESULT(t_const_char_ptr)	r;

	r = skip_word("hello'world");
	ASSERT_ERROR(r);
}

/* ── main ───────────────────────────────────────────────────────────────── */

int	main(void)
{
	RUN_TEST(test_word_before_operator);
	RUN_TEST(test_word_before_whitespace);
	RUN_TEST(test_word_before_pipe);
	RUN_TEST(test_word_before_redir_in);
	RUN_TEST(test_word_before_redir_out);
	RUN_TEST(test_word_before_heredoc);
	RUN_TEST(test_unclosed_double_quote);
	RUN_TEST(test_unclosed_single_quote);
	return (TEST_SUMMARY());
}
