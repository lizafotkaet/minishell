#include "debug_alloc.h"
#include "token.h"
#define TESTLIB_IMPL
#include "testlib.h"
#include "env.h"
#include <stdlib.h>
#include <string.h>

/* ── helpers ────────────────────────────────────────────────────────────── */
static bool	env_has_key_value(t_env env, const char *key, const char *value)
{
	int	i;

	i = 0;
	while (i < env.count)
	{
		if (m_env_key_value_pair_key_equals(env.pairs[i], key)
			&& strcmp(env.pairs[i].value, value) == 0)
			return (true);
		i++;
	}
	return (false);
}

/* ── test: parse_env from constant array ────────────────────────────────── */

void	test_parse_env(void)
{
	char			*envp[] = {"A=1", "B=2", "C=3", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(r.value.count == 3, "expected 3 pairs, got %d", r.value.count);
	ASSERT(r.value.capacity >= 3, "capacity too small: %d",
		r.value.capacity);
	ASSERT(strcmp(m_env_find_value(r.value, "A"), "1") == 0,
		"expected A=1");
	ASSERT(strcmp(m_env_find_value(r.value, "B"), "2") == 0,
		"expected B=2");
	ASSERT(strcmp(m_env_find_value(r.value, "C"), "3") == 0,
		"expected C=3");
	free_env(&r.value);
}

/* ── test: append to env ────────────────────────────────────────────────── */

void	test_env_append(void)
{
	char					*envp[] = {"X=10", NULL};
	RESULT(t_env)			r;
	RESULT(t_env_key_value_pair)	pr;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(r.value.count == 1, "initial count should be 1");
	pr = parse_env_var("Y=20");
	ASSERT_NO_ERROR(pr);
	ASSERT(m_env_append(&r.value, pr.value), "append should succeed");
	ASSERT(r.value.count == 2, "count should be 2 after append, got %d",
		r.value.count);
	ASSERT(strcmp(m_env_find_value(r.value, "X"), "10") == 0,
		"X should still be 10");
	ASSERT(strcmp(m_env_find_value(r.value, "Y"), "20") == 0,
		"Y should be 20");
	free_env(&r.value);
}

/* ── test: remove middle element ────────────────────────────────────────── */

void	test_env_remove_middle(void)
{
	char			*envp[] = {"A=1", "B=2", "C=3", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(m_env_remove(&r.value, "B"), "remove B should succeed");
	ASSERT(r.value.count == 2, "count should be 2, got %d", r.value.count);
	ASSERT(env_has_key_value(r.value, "A", "1"), "A=1 should remain");
	ASSERT(env_has_key_value(r.value, "C", "3"), "C=3 should remain");
	ASSERT(strcmp(m_env_find_value(r.value, "B"), "") == 0,
		"B should not be found");
	free_env(&r.value);
}

/* ── test: remove first element ─────────────────────────────────────────── */

void	test_env_remove_first(void)
{
	char			*envp[] = {"A=1", "B=2", "C=3", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(m_env_remove(&r.value, "A"), "remove A should succeed");
	ASSERT(r.value.count == 2, "count should be 2, got %d", r.value.count);
	ASSERT(env_has_key_value(r.value, "B", "2"), "B=2 should remain");
	ASSERT(env_has_key_value(r.value, "C", "3"), "C=3 should remain");
	ASSERT(strcmp(m_env_find_value(r.value, "A"), "") == 0,
		"A should not be found");
	free_env(&r.value);
}

/* ── test: remove last element ──────────────────────────────────────────── */

void	test_env_remove_last(void)
{
	char			*envp[] = {"A=1", "B=2", "C=3", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(m_env_remove(&r.value, "C"), "remove C should succeed");
	ASSERT(r.value.count == 2, "count should be 2, got %d", r.value.count);
	ASSERT(env_has_key_value(r.value, "A", "1"), "A=1 should remain");
	ASSERT(env_has_key_value(r.value, "B", "2"), "B=2 should remain");
	free_env(&r.value);
}

/* ── test: remove nonexistent key ───────────────────────────────────────── */

void	test_env_remove_nonexistent(void)
{
	char			*envp[] = {"A=1", "B=2", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(!m_env_remove(&r.value, "Z"), "remove Z should return false");
	ASSERT(r.value.count == 2, "count should still be 2");
	ASSERT(env_has_key_value(r.value, "A", "1"), "A=1 should remain");
	ASSERT(env_has_key_value(r.value, "B", "2"), "B=2 should remain");
	free_env(&r.value);
}

/* ── test: remove only element ──────────────────────────────────────────── */

void	test_env_remove_only(void)
{
	char			*envp[] = {"SOLO=val", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(m_env_remove(&r.value, "SOLO"), "remove SOLO should succeed");
	ASSERT(r.value.count == 0, "count should be 0, got %d", r.value.count);
	free_env(&r.value);
}

/* ── test: only correct element removed among duplicates ────────────────── */

void	test_env_remove_correct_only(void)
{
	char			*envp[] = {"KEY=first", "OTHER=x", "KEY=second", NULL};
	RESULT(t_env)	r;

	r = parse_env(envp);
	ASSERT_NO_ERROR(r);
	ASSERT(r.value.count == 2, "expected 2 pairs (dedup), got %d",
		r.value.count);
	ASSERT(strcmp(m_env_find_value(r.value, "KEY"), "second") == 0,
		"KEY should be second (last wins)");
	ASSERT(!env_has_key_value(r.value, "KEY", "first"),
		"KEY=first should not exist after dedup");
	ASSERT(env_has_key_value(r.value, "OTHER", "x"),
		"OTHER=x should remain");
	ASSERT(m_env_remove(&r.value, "KEY"), "remove KEY should succeed");
	ASSERT(r.value.count == 1, "count should be 1 after remove");
	ASSERT(strcmp(m_env_find_value(r.value, "KEY"), "") == 0,
		"KEY should not be found after remove");
	ASSERT(!m_env_remove(&r.value, "KEY"),
		"remove KEY again should return false");
	free_env(&r.value);
}

/* ── main ───────────────────────────────────────────────────────────────── */

int	main(void)
{
	RUN_TEST(test_parse_env);
	RUN_TEST(test_env_append);
	RUN_TEST(test_env_remove_middle);
	RUN_TEST(test_env_remove_first);
	RUN_TEST(test_env_remove_last);
	RUN_TEST(test_env_remove_nonexistent);
	RUN_TEST(test_env_remove_only);
	RUN_TEST(test_env_remove_correct_only);
	return (TEST_SUMMARY());
}
