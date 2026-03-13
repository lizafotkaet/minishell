#define TESTLIB_IMPL
#include "testlib.h"
#include "buffer.h"
#include "debug_alloc.h"
#include "../libft/libft.h"
#include <string.h>
#include <stdlib.h>

/* ── basic: new buffer is empty and null-terminated ─────────────────────── */
void	test_buffer_new(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	char				*s;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	ASSERT(buf.size == 0, "expected size 0, got %zu", buf.size);
	ASSERT(buf.capacity == BUFFER_INITIAL_CAPACITY,
		"expected capacity %d, got %zu",
		BUFFER_INITIAL_CAPACITY, buf.capacity);
	s = m_buffer_read(&buf);
	ASSERT(strcmp(s, "") == 0,
		"expected empty string, got \"%s\"", s);
	free(s);
	m_buffer_free(&buf);
}

/* ── write a short string and read it back ──────────────────────────────── */
void	test_buffer_write_read(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	bool				ok;
	char				*s;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	ok = m_buffer_write(&buf, "hello", 5);
	ASSERT(ok == true, "write returned false");
	ASSERT(buf.size == 5, "expected size 5, got %zu", buf.size);
	s = m_buffer_read(&buf);
	ASSERT(strcmp(s, "hello") == 0,
		"expected \"hello\", got \"%s\"", s);
	free(s);
	m_buffer_free(&buf);
}

/* ── multiple writes accumulate ─────────────────────────────────────────── */
void	test_buffer_multiple_writes(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	char				*s;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	m_buffer_write(&buf, "foo", 3);
	m_buffer_write(&buf, "bar", 3);
	m_buffer_write(&buf, "baz", 3);
	ASSERT(buf.size == 9, "expected size 9, got %zu", buf.size);
	s = m_buffer_read(&buf);
	ASSERT(strcmp(s, "foobarbaz") == 0,
		"expected \"foobarbaz\", got \"%s\"", s);
	free(s);
	m_buffer_free(&buf);
}

/* ── writing zero bytes is a no-op ──────────────────────────────────────── */
void	test_buffer_write_zero(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	char				*s;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	m_buffer_write(&buf, "anything", 0);
	ASSERT(buf.size == 0, "expected size 0, got %zu", buf.size);
	s = m_buffer_read(&buf);
	ASSERT(strcmp(s, "") == 0,
		"expected empty string, got \"%s\"", s);
	free(s);
	m_buffer_free(&buf);
}

/* ── x2 growth: write enough to force several reallocations ─────────────── */
void	test_buffer_growth(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	size_t				i;
	size_t				total;
	char				*s;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	total = BUFFER_INITIAL_CAPACITY * 8;
	i = 0;
	while (i < total)
	{
		m_buffer_write(&buf, "A", 1);
		i++;
	}
	ASSERT(buf.size == total, "expected size %zu, got %zu",
		total, buf.size);
	ASSERT(buf.capacity >= total + 1,
		"capacity %zu too small for size %zu",
		buf.capacity, total);
	s = m_buffer_read(&buf);
	i = 0;
	while (i < total)
	{
		ASSERT(s[i] == 'A',
			"expected 'A' at index %zu, got '%c'",
			i, s[i]);
		i++;
	}
	ASSERT(s[total] == '\0',
		"expected null terminator at index %zu", total);
	free(s);
	m_buffer_free(&buf);
}

/* ── null-termination is maintained after every write ───────────────────── */
void	test_buffer_null_terminated(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	char				*s;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	m_buffer_write(&buf, "ab", 2);
	ASSERT(buf.data[buf.size] == '\0',
		"not null-terminated after first write");
	m_buffer_write(&buf, "cd", 2);
	ASSERT(buf.data[buf.size] == '\0',
		"not null-terminated after second write");
	s = m_buffer_read(&buf);
	ASSERT(strcmp(s, "abcd") == 0,
		"expected \"abcd\", got \"%s\"", s);
	free(s);
	m_buffer_free(&buf);
}

/* ── read returns an independent copy ───────────────────────────────────── */
void	test_buffer_read_copies(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	char				*s1;
	char				*s2;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	m_buffer_write(&buf, "hello", 5);
	s1 = m_buffer_read(&buf);
	s2 = m_buffer_read(&buf);
	ASSERT(s1 != s2, "read should return distinct copies");
	ASSERT(strcmp(s1, s2) == 0, "copies should be equal");
	m_buffer_write(&buf, " world", 6);
	ASSERT(strcmp(s1, "hello") == 0,
		"old copy should be unaffected by further writes");
	free(s1);
	free(s2);
	m_buffer_free(&buf);
}

/* ── large single write triggers reallocation ───────────────────────────── */
void	test_buffer_large_single_write(void)
{
	RESULT(t_buffer)	r;
	t_buffer			buf;
	char				data[256];
	char				*s;
	size_t				i;

	r = m_buffer_new();
	ASSERT_NO_ERROR(r);
	buf = r.value;
	i = 0;
	while (i < sizeof(data))
	{
		data[i] = 'B';
		i++;
	}
	ASSERT(m_buffer_write(&buf, data, sizeof(data)) == true,
		"large write should succeed");
	ASSERT(buf.size == sizeof(data),
		"expected size %zu, got %zu", sizeof(data), buf.size);
	ASSERT(buf.capacity > BUFFER_INITIAL_CAPACITY,
		"expected reallocation, capacity %zu <= %d",
		buf.capacity, BUFFER_INITIAL_CAPACITY);
	s = m_buffer_read(&buf);
	ASSERT(s != NULL, "read should not return NULL");
	ASSERT(s[0] == 'B' && s[255] == 'B', "content mismatch");
	ASSERT(s[256] == '\0', "expected null terminator");
	free(s);
	m_buffer_free(&buf);
}

/* ── allocation failure returns ERROR ─────────────────────────────────── */
void	test_buffer_alloc_failure(void)
{
	RESULT(t_buffer)	r;

	SET_ALLOC_FAIL(1);
	r = m_buffer_new();
	ASSERT_ERROR(r);
	RESET_ALLOC_FAIL();
}

int	main(void)
{
	RUN_TEST(test_buffer_new);
	RUN_TEST(test_buffer_write_read);
	RUN_TEST(test_buffer_multiple_writes);
	RUN_TEST(test_buffer_write_zero);
	RUN_TEST(test_buffer_growth);
	RUN_TEST(test_buffer_null_terminated);
	RUN_TEST(test_buffer_read_copies);
	RUN_TEST(test_buffer_large_single_write);
	RUN_TEST(test_buffer_alloc_failure);
	return (TEST_SUMMARY());
}
