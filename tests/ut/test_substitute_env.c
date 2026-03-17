#define TESTLIB_IMPL
#include "testlib.h"
#include "env.h"
#include <stdlib.h>
#include <string.h>

/* ── shared env for most tests ──────────────────────────────────────────── */

static t_env_key_value_pair	g_pairs[] = {
	{"ROSES", "red"},
	{"VIOLETS", "blue"},
	{"EMPTY", ""},
	{"GREETING", "hello world"},
};
static t_env	g_env = {g_pairs, 4, 4, 0};

/* ── helper ─────────────────────────────────────────────────────────────── */

static void	assert_subst(const char *input, const char *expected,
		t_env env, const char *label)
{
	RESULT(t_char_ptr)	r;

	r = substitute_env(input, env);
	ASSERT(!r.is_error, "%s: expected SUCCESS, got ERROR", label);
	ASSERT(strcmp(r.value, expected) == 0,
		"%s: expected \"%s\", got \"%s\"", label, expected, r.value);
	free(r.value);
}

/* ── 1) main test from the spec ─────────────────────────────────────────── */
/*
** env: ROSES=red  VIOLETS=blue
**
** Raw token:
**   "$ROSES\""\ $VIOLETS\ '$A'\$ROSES$X
**
** Processing:
**   "$ROSES\""  → expand $ROSES→red, \" → literal ", remove quotes → red"
**   \ (space)   → literal space
**   $VIOLETS    → expand → blue
**   \ (space)   → literal space
**   '$A'        → single-quoted literal → $A
**   \$          → escaped $ → literal $
**   ROSES       → plain text
**   $X          → not in env → empty
**
** Result: red" blue $A$ROSES
*/
void	test_main_spec(void)
{
	assert_subst(
		"\"$ROSES\\\"\"\\ $VIOLETS\\ '$A'\\$ROSES$X",
		"red\" blue $A$ROSES",
		g_env, "main_spec");
}

/* ── 2) plain text, no quotes, no vars ──────────────────────────────────── */

void	test_plain_text(void)
{
	assert_subst("hello", "hello", g_env, "plain_text");
}

/* ── 3) simple $VAR expansion ───────────────────────────────────────────── */

void	test_simple_var(void)
{
	assert_subst("$ROSES", "red", g_env, "simple_var");
}

/* ── 4) unknown variable expands to empty ───────────────────────────────── */

void	test_unknown_var(void)
{
	assert_subst("$UNKNOWN", "", g_env, "unknown_var");
}

/* ── 5) single quotes suppress expansion ────────────────────────────────── */

void	test_single_quotes_no_expand(void)
{
	assert_subst("'$ROSES'", "$ROSES",
		g_env, "single_quotes_no_expand");
}

/* ── 6) double quotes allow expansion ───────────────────────────────────── */

void	test_double_quotes_expand(void)
{
	assert_subst("\"$ROSES\"", "red",
		g_env, "double_quotes_expand");
}

/* ── 7) escaped $ outside quotes → literal $ ────────────────────────────── */

void	test_escaped_dollar(void)
{
	assert_subst("\\$ROSES", "$ROSES",
		g_env, "escaped_dollar");
}

/* ── 8) escaped $ inside double quotes → literal $ ──────────────────────── */

void	test_escaped_dollar_in_dquotes(void)
{
	assert_subst("\"\\$ROSES\"", "$ROSES",
		g_env, "escaped_dollar_in_dquotes");
}

/* ── 9) escaped backslash inside double quotes → literal \ ──────────────── */

void	test_escaped_backslash_in_dquotes(void)
{
	assert_subst("\"\\\\\"", "\\",
		g_env, "escaped_backslash_in_dquotes");
}

/* ── 10) escaped quote inside double quotes → literal " ─────────────────── */

void	test_escaped_quote_in_dquotes(void)
{
	assert_subst("\"\\\"\"", "\"",
		g_env, "escaped_quote_in_dquotes");
}

/* ── 11) adjacent quoted and unquoted segments merge ────────────────────── */

void	test_adjacent_segments(void)
{
	assert_subst("'hello'\" world\"!", "hello world!",
		g_env, "adjacent_segments");
}

/* ── 12) empty variable value ───────────────────────────────────────────── */

void	test_empty_var(void)
{
	assert_subst("a$EMPTY.b", "a.b",
		g_env, "empty_var");
}

/* ── 13) $ followed by non-name char → literal $ ───────────────────────── */

void	test_dollar_no_name(void)
{
	assert_subst("$", "$", g_env, "dollar_no_name");
}

/* ── 14) $ followed by digit → literal $ + digit ───────────────────────── */

void	test_dollar_digit(void)
{
	assert_subst("$1abc", "$1abc", g_env, "dollar_digit");
}

/* ── 15) multiple vars in a row ─────────────────────────────────────────── */

void	test_multiple_vars(void)
{
	assert_subst("$ROSES$VIOLETS", "redblue",
		g_env, "multiple_vars");
}

/* ── 16) var with underscore in name ────────────────────────────────────── */

void	test_var_with_underscore(void)
{
	t_env_key_value_pair	pairs[] = {{"MY_VAR", "works"}};
	t_env					env;

	env.pairs = pairs;
	env.count = 1;
	env.capacity = 1;
	env.previous_command_exit_code = 0;
	assert_subst("$MY_VAR", "works", env, "var_with_underscore");
}

/* ── 17) backslash at end of unquoted (no char after) → literal \ ───────── */

void	test_trailing_backslash(void)
{
	assert_subst("abc\\", "abc\\",
		g_env, "trailing_backslash");
}

/* ── 18) value containing spaces via double quotes ──────────────────────── */

void	test_value_with_spaces(void)
{
	assert_subst("\"$GREETING\"", "hello world",
		g_env, "value_with_spaces");
}

/* ── 19) empty input ────────────────────────────────────────────────────── */

void	test_empty_input(void)
{
	assert_subst("", "", g_env, "empty_input");
}

/* ── 20) only quotes, no content ────────────────────────────────────────── */

void	test_only_quotes(void)
{
	assert_subst("\"\"''", "", g_env, "only_quotes");
}

/* ── 21) $? with exit code set ──────────────────────────────────────────── */

void	test_exit_code_set(void)
{
	t_env	env;

	env = g_env;
	m_env_set_exit_code(&env, 42);
	assert_subst("$?", "42", env, "exit_code_set");
}

/* ── 22) $? with exit code unset → default 0 ───────────────────────────── */

void	test_exit_code_unset(void)
{
	assert_subst("$?", "0", g_env, "exit_code_unset");
}

/* ── 23) $? mixed with other text ───────────────────────────────────────── */

void	test_exit_code_in_text(void)
{
	t_env	env;

	env = g_env;
	m_env_set_exit_code(&env, 127);
	assert_subst("exit:$?!", "exit:127!", env, "exit_code_in_text");
}

/* ── main ───────────────────────────────────────────────────────────────── */

int	main(void)
{
	RUN_TEST(test_main_spec);
	RUN_TEST(test_plain_text);
	RUN_TEST(test_simple_var);
	RUN_TEST(test_unknown_var);
	RUN_TEST(test_single_quotes_no_expand);
	RUN_TEST(test_double_quotes_expand);
	RUN_TEST(test_escaped_dollar);
	RUN_TEST(test_escaped_dollar_in_dquotes);
	RUN_TEST(test_escaped_backslash_in_dquotes);
	RUN_TEST(test_escaped_quote_in_dquotes);
	RUN_TEST(test_adjacent_segments);
	RUN_TEST(test_empty_var);
	RUN_TEST(test_dollar_no_name);
	RUN_TEST(test_dollar_digit);
	RUN_TEST(test_multiple_vars);
	RUN_TEST(test_var_with_underscore);
	RUN_TEST(test_trailing_backslash);
	RUN_TEST(test_value_with_spaces);
	RUN_TEST(test_empty_input);
	RUN_TEST(test_only_quotes);
	RUN_TEST(test_exit_code_set);
	RUN_TEST(test_exit_code_unset);
	RUN_TEST(test_exit_code_in_text);
	return (TEST_SUMMARY());
}
