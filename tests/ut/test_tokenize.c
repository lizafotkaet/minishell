#include "token.h"
#define TESTLIB_IMPL
#include "testlib.h"
#include "tokenize.h"
#include "debug_alloc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* helper: compare token vector content strings against expected */
static void	assert_tokens(t_token_vector *vec, char **expected,
		const char *label)
{
	size_t	i;

	i = 0;
	while (expected[i] != NULL)
	{
		ASSERT(i < vec->size,
			"%s: expected more tokens at index %zu", label, i);
		ASSERT(strcmp(vec->data[i].content, expected[i]) == 0,
			"%s[%zu]: expected \"%s\", got \"%s\"",
			label, i, expected[i], vec->data[i].content);
		i++;
	}
	ASSERT(i == vec->size,
		"%s: expected %zu tokens, got %zu", label, i, vec->size);
}

/* helper: assert token types against expected array */
static void	assert_types(t_token_vector *vec, t_token_type *expected,
		size_t count, const char *label)
{
	size_t	i;

	ASSERT(vec->size == count,
		"%s: expected %zu tokens, got %zu", label, count, vec->size);
	i = 0;
	while (i < count)
	{
		ASSERT(vec->data[i].token_type == expected[i],
			"%s[%zu]: expected type %d, got %d",
			label, i, expected[i], vec->data[i].token_type);
		i++;
	}
}

/* ── 1) four-token "Sixteen Tons" line ──────────────────────────────────── */

void	test_sixteen_tons(void)
{
	const char				*input;
	RESULT(t_token_vector)	r;
	char					*expected[] = {
		"\"Some\"'people'say\\ a\\ man\"is made \\\\ outta mud \\\"\"",
		"A\\ poor\\ \"man's\"'made'\"outta muscle n' blood\"",
		">",
		"'Sixteen Tons'",
		NULL
	};
	t_token_type			types[] = {
		E_WORD, E_WORD, E_REDIR_OUT, E_WORD};

	input =
		"\"Some\"'people'say\\ a\\ man\"is made \\\\ outta mud \\\"\""
		" \t \t "
		"A\\ poor\\ \"man's\"'made'\"outta muscle n' blood\""
		" "
		">"
		" "
		"'Sixteen Tons'";
	r = tokenize_string(input);
	ASSERT_NO_ERROR(r);
	assert_tokens(&r.value, expected, "sixteen_tons");
	assert_types(&r.value, types, 4, "sixteen_tons_types");
	m_token_vector_free(&r.value);
}

/* ── unclosed double-quote ──────────────────────────────────────────────── */

void	test_unclosed_double(void)
{
	RESULT(t_token_vector)	r;

	r = tokenize_string("hello \"world");
	ASSERT_ERROR(r);
}

/* ── unclosed single-quote ──────────────────────────────────────────────── */

void	test_unclosed_single(void)
{
	RESULT(t_token_vector)	r;

	r = tokenize_string("hello 'world");
	ASSERT_ERROR(r);
}

/* ── 2) heredoc: three tokens with escaped operators ────────────────────── */

void	test_heredoc_operators(void)
{
	const char				*input;
	RESULT(t_token_vector)	r;
	char					*expected[] = {
		"\"><>>/\"'><>>|'\\>\\|\\<",
		">>",
		"\"HEREDOC CHARACTER\"",
		NULL
	};
	t_token_type			types[] = {
		E_WORD, E_REDIR_APPEND, E_WORD};

	input = "\"><>>/\"'><>>|'\\>\\|\\<>>\"HEREDOC CHARACTER\"";
	r = tokenize_string(input);
	ASSERT_NO_ERROR(r);
	assert_tokens(&r.value, expected, "heredoc_operators");
	assert_types(&r.value, types, 3, "heredoc_operators_types");
	m_token_vector_free(&r.value);
}

/* ── 3) two words separated by whitespace ───────────────────────────────── */

void	test_two_words_whitespace(void)
{
	RESULT(t_token_vector)	r;
	char					*expected[] = {"hello", "world", NULL};
	t_token_type			types[] = {E_WORD, E_WORD};

	r = tokenize_string("hello   \t  world");
	ASSERT_NO_ERROR(r);
	assert_tokens(&r.value, expected, "two_words");
	assert_types(&r.value, types, 2, "two_words_types");
	m_token_vector_free(&r.value);
}

/* ── 4) AAA>'' → three tokens with types ────────────────────────────────── */

void	test_aaa_redirect_empty(void)
{
	RESULT(t_token_vector)	r;
	char					*expected[] = {"AAA", ">", "''", NULL};
	t_token_type			types[] = {E_WORD, E_REDIR_OUT, E_WORD};

	r = tokenize_string("AAA>''");
	ASSERT_NO_ERROR(r);
	assert_tokens(&r.value, expected, "aaa_redirect");
	assert_types(&r.value, types, 3, "aaa_redirect_types");
	m_token_vector_free(&r.value);
}

/* ── 5) pipe and heredoc operator types ─────────────────────────────────── */

void	test_operator_types(void)
{
	RESULT(t_token_vector)	r;
	t_token_type			types[] = {
		E_WORD, E_PIPE, E_WORD, E_HEREDOC, E_WORD,
		E_REDIR_IN, E_WORD};

	r = tokenize_string("echo | cat << EOF < infile");
	ASSERT_NO_ERROR(r);
	assert_types(&r.value, types, 7, "operator_types");
	m_token_vector_free(&r.value);
}

/*  ── 6) Test redirect after word ────────────────────────────────────────── */

void test_redirect_after_word(void)
{
	RESULT(t_token_vector)	r;
	char					*expected[] = {
		"echo",
		">",
		"file.txt",
		">",
		"file2.txt",
		"xxxx",
		">",
		"file3.txt\">\"",
		NULL};
	t_token_type			types[] = {
		E_WORD,
		E_REDIR_OUT,
		E_WORD,
		E_REDIR_OUT,
		E_WORD,
		E_WORD,
		E_REDIR_OUT,
		E_WORD,
	};

	r = tokenize_string("echo     >file.txt>file2.txt xxxx>file3.txt\">\"");
	ASSERT_NO_ERROR(r);
	assert_tokens(&r.value, expected, "redirect_after_word");
	assert_types(&r.value, types, 8, "redirect_after_word_types");
	m_token_vector_free(&r.value);
}

/* ── 7) Test trailing spaces ─────────────────────────────────────────────── */

void test_trailing_spaces(void)
{
	RESULT(t_token_vector)	r;
	char					*expected[] = {"echo", NULL};
	t_token_type			types[] = {E_WORD};

	r = tokenize_string("       \t     echo    \t   ");
	ASSERT_NO_ERROR(r);
	assert_tokens(&r.value, expected, "trailing_spaces");
	assert_types(&r.value, types, 1, "trailing_spaces_types");
	m_token_vector_free(&r.value);
}

/* ── 8) Many tokens trigger vector reallocation ─────────────────────────────── */

void	test_many_tokens(void)
{
	RESULT(t_token_vector)	r;
	char					input[512];
	int						pos;
	size_t					total;
	size_t					i;

	total = TOKEN_VECTOR_INITIAL_CAPACITY + 8;
	pos = 0;
	i = 0;
	while (i < total)
	{
		if (i > 0)
			input[pos++] = ' ';
		pos += sprintf(input + pos, "w%zu", i);
		i++;
	}
	input[pos] = '\0';
	r = tokenize_string(input);
	ASSERT_NO_ERROR(r);
	ASSERT(r.value.size == total,
		"expected %zu tokens, got %zu", total, r.value.size);
	ASSERT(r.value.capacity > TOKEN_VECTOR_INITIAL_CAPACITY,
		"expected realloc, capacity %zu <= %d",
		r.value.capacity, TOKEN_VECTOR_INITIAL_CAPACITY);
	m_token_vector_free(&r.value);
}

/* ── 9) Allocation failure in tokenize ─────────────────────────────────── */

void	test_tokenize_alloc_failure(void)
{
	RESULT(t_token_vector)	r;

	SET_ALLOC_FAIL(1);
	r = tokenize_string("hello world");
	ASSERT_ERROR(r);
	RESET_ALLOC_FAIL();
}

/* ── main ───────────────────────────────────────────────────────────────── */

int	main(void)
{
	RUN_TEST(test_sixteen_tons);
	RUN_TEST(test_unclosed_double);
	RUN_TEST(test_unclosed_single);
	RUN_TEST(test_heredoc_operators);
	RUN_TEST(test_two_words_whitespace);
	RUN_TEST(test_aaa_redirect_empty);
	RUN_TEST(test_operator_types);
	RUN_TEST(test_redirect_after_word);
	RUN_TEST(test_trailing_spaces);
	RUN_TEST(test_many_tokens);
	RUN_TEST(test_tokenize_alloc_failure);
	return (TEST_SUMMARY());
}
