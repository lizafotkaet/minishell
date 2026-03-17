#define TESTLIB_IMPL
#include "testlib.h"
#include "string_vector.h"
#include <string.h>
#include <stdlib.h>

/* ── basic: new vector is empty ─────────────────────────────────────────── */
void	test_vector_new(void)
{
	RESULT(t_string_vector_ptr)	r;
	t_string_vector				*vec;

	r = m_string_vector_new();
	ASSERT_NO_ERROR(r);
	vec = r.value;
	ASSERT(vec->size == 0, "expected size 0, got %zu", vec->size);
	ASSERT(vec->capacity == STRING_VECTOR_INITIAL_CAPACITY,
		"expected capacity %d, got %zu",
		STRING_VECTOR_INITIAL_CAPACITY, vec->capacity);
	ASSERT(vec->data[0] == NULL, "data[0] should be NULL");
	m_string_vector_free(vec);
}

/* ── append one string and read it back ─────────────────────────────────── */
void	test_vector_append_one(void)
{
	RESULT(t_string_vector_ptr)	r;
	t_string_vector				*vec;
	bool						ok;

	r = m_string_vector_new();
	ASSERT_NO_ERROR(r);
	vec = r.value;
	ok = m_string_vector_append(vec, "hello");
	ASSERT(ok == true, "append returned false");
	ASSERT(vec->size == 1, "expected size 1, got %zu", vec->size);
	ASSERT(strcmp(m_string_vector_at(vec, 0), "hello") == 0,
		"expected \"hello\" at index 0, got \"%s\"",
		m_string_vector_at(vec, 0));
	ASSERT(vec->data[vec->size] == NULL,
		"data[size] should be NULL terminator");
	m_string_vector_free(vec);
}

/* ── append several strings ─────────────────────────────────────────────── */
void	test_vector_append_multiple(void)
{
	RESULT(t_string_vector_ptr)	r;
	t_string_vector				*vec;

	r = m_string_vector_new();
	ASSERT_NO_ERROR(r);
	vec = r.value;
	m_string_vector_append(vec, "alpha");
	m_string_vector_append(vec, "beta");
	m_string_vector_append(vec, "gamma");
	ASSERT(vec->size == 3, "expected size 3, got %zu", vec->size);
	ASSERT(strcmp(m_string_vector_at(vec, 0), "alpha") == 0,
		"index 0: expected \"alpha\", got \"%s\"",
		m_string_vector_at(vec, 0));
	ASSERT(strcmp(m_string_vector_at(vec, 1), "beta") == 0,
		"index 1: expected \"beta\", got \"%s\"",
		m_string_vector_at(vec, 1));
	ASSERT(strcmp(m_string_vector_at(vec, 2), "gamma") == 0,
		"index 2: expected \"gamma\", got \"%s\"",
		m_string_vector_at(vec, 2));
	ASSERT(vec->data[vec->size] == NULL,
		"data[size] should be NULL terminator");
	m_string_vector_free(vec);
}

/* ── at returns NULL for out-of-bounds index ────────────────────────────── */
void	test_vector_at_out_of_bounds(void)
{
	RESULT(t_string_vector_ptr)	r;
	t_string_vector				*vec;

	r = m_string_vector_new();
	ASSERT_NO_ERROR(r);
	vec = r.value;
	m_string_vector_append(vec, "only");
	ASSERT(m_string_vector_at(vec, 0) != NULL,
		"at(0) should not be NULL");
	ASSERT(m_string_vector_at(vec, 1) == NULL,
		"at(1) should be NULL for size=1");
	ASSERT(m_string_vector_at(vec, 100) == NULL,
		"at(100) should be NULL");
	m_string_vector_free(vec);
}

/* ── x2 growth: append enough to force several reallocations ────────────── */
void	test_vector_growth(void)
{
	RESULT(t_string_vector_ptr)	r;
	t_string_vector				*vec;
	size_t						total;
	size_t						i;
	char						num[16];

	r = m_string_vector_new();
	ASSERT_NO_ERROR(r);
	vec = r.value;
	total = STRING_VECTOR_INITIAL_CAPACITY * 8;
	i = 0;
	while (i < total)
	{
		sprintf(num, "%zu", i);
		m_string_vector_append(vec, num);
		i++;
	}
	ASSERT(vec->size == total, "expected size %zu, got %zu",
		total, vec->size);
	ASSERT(vec->capacity >= total + 1,
		"capacity %zu too small for size %zu",
		vec->capacity, total);
	/* spot-check first and last */
	ASSERT(strcmp(m_string_vector_at(vec, 0), "0") == 0,
		"at(0): expected \"0\", got \"%s\"",
		m_string_vector_at(vec, 0));
	sprintf(num, "%zu", total - 1);
	ASSERT(strcmp(m_string_vector_at(vec, total - 1), num) == 0,
		"at(%zu): expected \"%s\", got \"%s\"",
		total - 1, num, m_string_vector_at(vec, total - 1));
	ASSERT(vec->data[vec->size] == NULL,
		"data[size] should be NULL terminator");
	m_string_vector_free(vec);
}

/* ── append makes a copy (original can be freed) ────────────────────────── */
void	test_vector_append_copies(void)
{
	RESULT(t_string_vector_ptr)	r;
	t_string_vector				*vec;
	char						*s;

	r = m_string_vector_new();
	ASSERT_NO_ERROR(r);
	vec = r.value;
	s = strdup("original");
	m_string_vector_append(vec, s);
	free(s);
	ASSERT(strcmp(m_string_vector_at(vec, 0), "original") == 0,
		"expected \"original\" after freeing source, got \"%s\"",
		m_string_vector_at(vec, 0));
	m_string_vector_free(vec);
}

int	main(void)
{
	RUN_TEST(test_vector_new);
	RUN_TEST(test_vector_append_one);
	RUN_TEST(test_vector_append_multiple);
	RUN_TEST(test_vector_at_out_of_bounds);
	RUN_TEST(test_vector_growth);
	RUN_TEST(test_vector_append_copies);
	return (TEST_SUMMARY());
}
