#include "debug_alloc.h"
#include "pipeline.h"
#define TESTLIB_IMPL
#include "testlib.h"
#include <stdlib.h>
#include <string.h>

/* ── shared env for tests ───────────────────────────────────────────────── */

static t_env_key_value_pair	g_pairs[] = {
	{"USER", "alice"},
	{"HOME", "/home/alice"},
};
static t_env	g_env = {g_pairs, 2, 2, 0};

/* ── helpers ────────────────────────────────────────────────────────────── */

static void	assert_pipeline_error(const char *input, const char *label)
{
	RESULT(t_pipeline)	r;

	r = create_pipeline(input, g_env);
	ASSERT(r.is_error, "%s: expected ERROR, got SUCCESS", label);
}

/* ── 1) simple command: echo hello ──────────────────────────────────────── */

void	test_simple_command(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("echo hello", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->command_count == 1, "expected 1 command, got %d",
		pl->command_count);
	ASSERT(pl->commands[0].argc == 2, "expected argc=2, got %d",
		pl->commands[0].argc);
	ASSERT(strcmp(pl->commands[0].argv[0], "echo") == 0,
		"expected argv[0]=\"echo\", got \"%s\"",
		pl->commands[0].argv[0]);
	ASSERT(strcmp(pl->commands[0].argv[1], "hello") == 0,
		"expected argv[1]=\"hello\", got \"%s\"",
		pl->commands[0].argv[1]);
	ASSERT(pl->commands[0].argv[2] == NULL,
		"expected argv[2]=NULL");
	ASSERT(pl->commands[0].redirect_count == 0,
		"expected 0 redirects");
	free_pipeline(pl);
}

/* ── 2) pipe: ls | grep foo ─────────────────────────────────────────────── */

void	test_pipe(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("ls | grep foo", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->command_count == 2, "expected 2 commands, got %d",
		pl->command_count);
	ASSERT(strcmp(pl->commands[0].argv[0], "ls") == 0,
		"cmd0: expected \"ls\"");
	ASSERT(strcmp(pl->commands[1].argv[0], "grep") == 0,
		"cmd1: expected \"grep\"");
	ASSERT(strcmp(pl->commands[1].argv[1], "foo") == 0,
		"cmd1: expected argv[1]=\"foo\"");
	free_pipeline(pl);
}

/* ── 3) redirect out: echo hi > out.txt ─────────────────────────────────── */

void	test_redirect_out(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("echo hi > out.txt", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->commands[0].argc == 2, "expected argc=2, got %d",
		pl->commands[0].argc);
	ASSERT(pl->commands[0].redirect_count == 1,
		"expected 1 redirect, got %d",
		pl->commands[0].redirect_count);
	ASSERT(pl->commands[0].redirects[0].type == E_REDIR_OUT,
		"expected REDIR_OUT");
	ASSERT(strcmp(pl->commands[0].redirects[0].target, "out.txt") == 0,
		"expected target \"out.txt\"");
	free_pipeline(pl);
}

/* ── 4) redirect in: cat < in.txt ───────────────────────────────────────── */

void	test_redirect_in(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("cat < in.txt", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->commands[0].argc == 1, "expected argc=1");
	ASSERT(pl->commands[0].redirect_count == 1, "expected 1 redirect");
	ASSERT(pl->commands[0].redirects[0].type == E_REDIR_IN,
		"expected REDIR_IN");
	ASSERT(strcmp(pl->commands[0].redirects[0].target, "in.txt") == 0,
		"expected target \"in.txt\"");
	free_pipeline(pl);
}

/* ── 5) heredoc: cat << EOF ─────────────────────────────────────────────── */

void	test_heredoc(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("cat << EOF", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->commands[0].argc == 1, "expected argc=1");
	ASSERT(pl->commands[0].redirect_count == 1, "expected 1 redirect");
	ASSERT(pl->commands[0].redirects[0].type == E_HEREDOC,
		"expected HEREDOC");
	ASSERT(strcmp(pl->commands[0].redirects[0].target, "EOF") == 0,
		"expected target \"EOF\"");
	free_pipeline(pl);
}

/* ── 6) append redirect: echo hi >> log ─────────────────────────────────── */

void	test_redirect_append(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("echo hi >> log", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->commands[0].redirect_count == 1, "expected 1 redirect");
	ASSERT(pl->commands[0].redirects[0].type == E_REDIR_APPEND,
		"expected REDIR_APPEND");
	free_pipeline(pl);
}

/* ── 7) multiple redirects: cmd < in > out >> app ───────────────────────── */

void	test_multiple_redirects(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("cmd < in > out >> app", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->commands[0].argc == 1, "expected argc=1");
	ASSERT(pl->commands[0].redirect_count == 3,
		"expected 3 redirects, got %d",
		pl->commands[0].redirect_count);
	ASSERT(pl->commands[0].redirects[0].type == E_REDIR_IN,
		"redir[0] type");
	ASSERT(pl->commands[0].redirects[1].type == E_REDIR_OUT,
		"redir[1] type");
	ASSERT(pl->commands[0].redirects[2].type == E_REDIR_APPEND,
		"redir[2] type");
	free_pipeline(pl);
}

/* ── 8) env expansion in pipeline: echo $USER ───────────────────────────── */

void	test_env_expansion(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("echo $USER", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(strcmp(pl->commands[0].argv[1], "alice") == 0,
		"expected \"alice\", got \"%s\"",
		pl->commands[0].argv[1]);
	free_pipeline(pl);
}

/* ── 9) three-stage pipe: ls | grep a | wc -l ──────────────────────────── */

void	test_three_pipes(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("ls | grep a | wc -l", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->command_count == 3, "expected 3 commands, got %d",
		pl->command_count);
	ASSERT(strcmp(pl->commands[2].argv[0], "wc") == 0,
		"cmd2 argv[0]");
	ASSERT(strcmp(pl->commands[2].argv[1], "-l") == 0,
		"cmd2 argv[1]");
	free_pipeline(pl);
}

/* ── 10) redirect before command: > out echo hi ─────────────────────────── */

void	test_redirect_before_cmd(void)
{
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;

	r = create_pipeline("> out echo hi", g_env);
	ASSERT_NO_ERROR(r);
	pl = &r.value;
	ASSERT(pl->commands[0].argc == 2, "expected argc=2, got %d",
		pl->commands[0].argc);
	ASSERT(pl->commands[0].redirect_count == 1, "expected 1 redirect");
	ASSERT(strcmp(pl->commands[0].argv[0], "echo") == 0,
		"expected argv[0]=\"echo\"");
	free_pipeline(pl);
}

/* ── error: empty input ─────────────────────────────────────────────────── */

void	test_error_empty(void)
{
	assert_pipeline_error("", "empty_input");
}

/* ── error: only pipe ───────────────────────────────────────────────────── */

void	test_error_only_pipe(void)
{
	assert_pipeline_error("|", "only_pipe");
}

/* ── error: pipe at start ───────────────────────────────────────────────── */

void	test_error_pipe_at_start(void)
{
	assert_pipeline_error("| echo", "pipe_at_start");
}

/* ── error: pipe at end ─────────────────────────────────────────────────── */

void	test_error_pipe_at_end(void)
{
	assert_pipeline_error("echo |", "pipe_at_end");
}

/* ── error: redirect without target ─────────────────────────────────────── */

void	test_error_redirect_no_target(void)
{
	assert_pipeline_error("echo >", "redirect_no_target");
}

/* ── error: redirect followed by operator ───────────────────────────────── */

void	test_error_redirect_then_pipe(void)
{
	assert_pipeline_error("echo > | cat", "redirect_then_pipe");
}

/* ── error: only redirects, no command word ──────────────────────────────── */

void	test_error_only_redirects(void)
{
	assert_pipeline_error("> out < in", "only_redirects");
}

/* ── unclosed quotes: "hello\\\" → 1 backslash (odd) → escaped → unclosed  */
void	test_unclosed_double(void)
{
	assert_pipeline_error("echo \"hello\\\"", "unclosed_double");
}

/* ── large string with many tokens (test reallocations) ───────────────────────────── */
void test_large(void)
{
	// "cmd a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 a16 a17 a18 a19 a20 a21 a22 a23 a24 a25 a26 a27 a28 a29 a30 a31 a32 a33 a34 a35 a36 a37 a38 a39 a40 a41 a42 a43 a44 a45 a46 a47 a48 a49 a50 a51 a52 a53 a54 a55 a56 a57 a58 a59 a60 a>b \"aa\"bb'cc\"dd\"ee'\\\\\\ff$a#x | cat < in.txt >> out.txt << EOF'ddd' | grep foo | wc -l --example.com.dd.rm $a$b$c$d$e$f$g$h$i$j$k$l$m$n$o$p$q$r$s$t$u$v$w$x$y$z | echo done|echo done2 > a > b>c>d>  \t\te    e\"'\"'\\'>  dd << INPUT | x1 x2 3 4 5 6 7 8 9 1 0a>b <e >f>f>g>h>e\"<><><<<<>>><<><<>><><><\"'<<'>a\\>\\<1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 & x d f | a1 | a2 | a3 | a4 | a5 | a6 | a7 | a8 | a9 | a10 | a11 | a12 | a13 | a14 | a15 | a16 | a17 | a18 | a19 | a20 | a21 | a22 | a23 | a24 | echo \\ 'done' ' | ' \\| \" dododo | >\"";
	const char * input = "cmd a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 a16"
		" a17 a18 a19 a20 a21 a22 a23 a24 a25 a26 a27 a28 a29 a30"
		" a31 a32 a33 a34 a35 a36 a37 a38 a39 a40 a41 a42 a43 a44 a45"
		" a46 a47 a48 a49 a50 a51 a52 a53 a54 a55 a56 a57 a58 a59 a60"
		" a>b \"aa\"bb'cc\"dd\"ee'\\\\\\ff$a#x | " // 65 tokens and one pipe
		"cat < in.txt >> out.txt << EOF'ddd'"
		" | grep foo | wc -l --example.com.dd.rm $a$b$c$d$e$f$g$h$i$j$k$l$m$n$o$"
		"p$q$r$s$t$u$v$w$x$y$z | echo done|echo done2 > a > b>c>d>  \t\te"
		"    e\"'\"'\\'>  dd << INPUT | x1 x2 3 4 5 6 7 8 9 1 0"
		"a>b <e >f>f>g>h>e\"<><><<<<>>><<><<>><><><\"'<<'>a\\>\\<"
		"1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 "
		"1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 "
		"1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 "
		"1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 "
		"& x d f | a1 | a2 | a3 | a4 | a5 | a6 | a7 | a8 | a9 | a10 | "
		"a11 | a12 | a13 | a14 | a15 | a16 | a17 | a18 | a19 | a20 | "
		"a21 | a22 | a23 | a24 | echo \\ 'done' ' | ' \\| \" dododo | >\"";

	bool success = false;
	int i = 1;
	RESULT(t_pipeline)	r;
	t_pipeline			*pl;
	static t_env_key_value_pair	g_pairs[] = {
		{"a", "aa"},
		{"b", "bb"},
		{"c", "cc"},
		{"d", "dd"},
		{"e", "ee"},
		{"f", "ff"},
		{"g", "gg"},
		{"h", "hh"},
		{"i", "ii"},
		{"j", "jj"},
		{"k", "kk"},
		{"l", "ll"},
		{"m", "mm"},
		{"n", "nn"},
		{"o", "oo"},
	};
	static t_env	g_env = {g_pairs, 15, 16, 0};

	while (!success)
	{
		SET_ALLOC_FAIL(i);
		r = create_pipeline(input, g_env);
		if (r.is_error)
		{
			free_pipeline(&r.value);
		}
		RESET_ALLOC_FAIL();
		success = !r.is_error;
		i++;
	}
	pl = &r.value;
	ASSERT(i > 200, "Expected to test at least 200 allocations, got %d", i);
	ASSERT(pl->command_count == 32, "expected 32 commands, got %d", pl->command_count);
	char * expected_first_cmd_argv[] = {"cmd", "a1", "a2", "a3", "a4", "a5",
		"a6", "a7", "a8", "a9", "a10", "a11", "a12", "a13", "a14", "a15",
		"a16", "a17", "a18", "a19", "a20", "a21", "a22", "a23", "a24",
		"a25", "a26", "a27", "a28", "a29", "a30", "a31", "a32",
		"a33", "a34", "a35", "a36", "a37", "a38", "a39", "a40", "a41",
		"a42", "a43", "a44", "a45", "a46", "a47", "a48", "a49", "a50", "a51",
		"a52", "a53", "a54", "a55", "a56", "a57", "a58", "a59", "a60", "a",
		"aabbcc\"dd\"ee\\ffaa#x", NULL};
	ASSERT_STR_ARR_EQ(pl->commands[0].argv, expected_first_cmd_argv);
	ASSERT(
		pl->commands[0].redirect_count == 1,
		"Expected 1 redirect in cmd0, got %d",
		pl->commands[0].redirect_count
	);
	ASSERT(
		pl->commands[0].redirects[0].type == E_REDIR_OUT,
		"Expected REDIR_OUT in cmd0, got %d",
		pl->commands[0].redirects[0].type
	);
	ASSERT(strcmp(pl->commands[0].redirects[0].target, "b") == 0,
		"Expected redirect target \"b\" in cmd0, got \"%s\"",
		pl->commands[0].redirects[0].target);
	// cat < in.txt >> out.txt << EOF'ddd'
	char * expected_second_cmd_argv[] = {"cat", NULL};
	ASSERT_STR_ARR_EQ(pl->commands[1].argv, expected_second_cmd_argv);
	ASSERT(pl->commands[1].redirect_count == 3, "Expected 3 redirects in cmd1, got %d", pl->commands[1].redirect_count);
	ASSERT(pl->commands[1].redirects[0].type == E_REDIR_IN, "Expected REDIR_IN in cmd1 redir0, got %d", pl->commands[1].redirects[0].type);
	ASSERT(strcmp(pl->commands[1].redirects[0].target, "in.txt") == 0, "Expected redirect target \"in.txt\" in cmd1 redir0, got \"%s\"", pl->commands[1].redirects[0].target);
	ASSERT(pl->commands[1].redirects[1].type == E_REDIR_APPEND, "Expected REDIR_APPEND in cmd1 redir1, got %d", pl->commands[1].redirects[1].type);
	ASSERT(strcmp(pl->commands[1].redirects[1].target, "out.txt") == 0, "Expected redirect target \"out.txt\" in cmd1 redir1, got \"%s\"", pl->commands[1].redirects[1].target);
	ASSERT(pl->commands[1].redirects[2].type == E_HEREDOC, "Expected HEREDOC in cmd1 redir2, got %d", pl->commands[1].redirects[2].type);
	ASSERT(strcmp(pl->commands[1].redirects[2].target, "EOFddd") == 0, "Expected redirect target \"EOFddd\" in cmd1 redir2, got \"%s\"", pl->commands[1].redirects[2].target);

	free_pipeline(&r.value);
}

/* ── main ───────────────────────────────────────────────────────────────── */

int	main(void)
{
	RUN_TEST(test_simple_command);
	RUN_TEST(test_pipe);
	RUN_TEST(test_redirect_out);
	RUN_TEST(test_redirect_in);
	RUN_TEST(test_heredoc);
	RUN_TEST(test_redirect_append);
	RUN_TEST(test_multiple_redirects);
	RUN_TEST(test_env_expansion);
	RUN_TEST(test_three_pipes);
	RUN_TEST(test_redirect_before_cmd);
	RUN_TEST(test_error_empty);
	RUN_TEST(test_error_only_pipe);
	RUN_TEST(test_error_pipe_at_start);
	RUN_TEST(test_error_pipe_at_end);
	RUN_TEST(test_error_redirect_no_target);
	RUN_TEST(test_error_redirect_then_pipe);
	RUN_TEST(test_error_only_redirects);
	RUN_TEST(test_unclosed_double);
	RUN_TEST(test_large);
	return (TEST_SUMMARY());
}
