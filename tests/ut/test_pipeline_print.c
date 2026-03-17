#include "debug_alloc.h"
#include "pipeline_print.h"
#define TESTLIB_IMPL
#include "testlib.h"
#include <stdlib.h>
#include <string.h>

static t_env_key_value_pair	g_pairs[] = {
	{"USER", "alice"},
};
static t_env	g_env = {g_pairs, 1, 1, 0};

void	test_simple_command_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;
	const char			*expected;

	pr = create_pipeline("ls -la", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	expected = "<pipeline>\n"
		"\t<command>\n"
		"\t\t<args>\n"
		"\t\t\t<arg>ls</arg>\n"
		"\t\t\t<arg>-la</arg>\n"
		"\t\t</args>\n"
		"\t</command>\n"
		"</pipeline>\n";
	ASSERT(strcmp(xr.value, expected) == 0,
		"got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

void	test_redirect_out_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;
	const char			*expected;

	pr = create_pipeline("echo hi > out.txt", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	expected = "<pipeline>\n"
		"\t<command>\n"
		"\t\t<args>\n"
		"\t\t\t<arg>echo</arg>\n"
		"\t\t\t<arg>hi</arg>\n"
		"\t\t</args>\n"
		"\t\t<redirects>\n"
		"\t\t\t<redir kind=\"out\" dst=\"out.txt\" />\n"
		"\t\t</redirects>\n"
		"\t</command>\n"
		"</pipeline>\n";
	ASSERT(strcmp(xr.value, expected) == 0,
		"got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

void	test_redirect_in_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;

	pr = create_pipeline("cat < input.txt", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	ASSERT(strstr(xr.value, "kind=\"in\" dst=\"input.txt\"") != NULL,
		"expected redir in, got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

void	test_redirect_append_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;

	pr = create_pipeline("echo hi >> log.txt", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	ASSERT(strstr(xr.value, "kind=\"append\" dst=\"log.txt\"") != NULL,
		"expected redir append, got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

void	test_pipe_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;
	const char			*expected;

	pr = create_pipeline("ls | wc -l", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	expected = "<pipeline>\n"
		"\t<command>\n"
		"\t\t<args>\n"
		"\t\t\t<arg>ls</arg>\n"
		"\t\t</args>\n"
		"\t</command>\n"
		"\t<command>\n"
		"\t\t<args>\n"
		"\t\t\t<arg>wc</arg>\n"
		"\t\t\t<arg>-l</arg>\n"
		"\t\t</args>\n"
		"\t</command>\n"
		"</pipeline>\n";
	ASSERT(strcmp(xr.value, expected) == 0,
		"got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

void	test_env_expansion_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;

	pr = create_pipeline("echo $USER", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	ASSERT(strstr(xr.value, "<arg>alice</arg>") != NULL,
		"expected expanded var, got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

void	test_multiple_redirects_xml(void)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;

	pr = create_pipeline("cmd < in.txt > out.txt", g_env);
	ASSERT_NO_ERROR(pr);
	xr = pipeline_to_xml(&pr.value);
	ASSERT_NO_ERROR(xr);
	ASSERT(strstr(xr.value, "kind=\"in\" dst=\"in.txt\"") != NULL,
		"expected redir in, got:\n%s", xr.value);
	ASSERT(strstr(xr.value, "kind=\"out\" dst=\"out.txt\"") != NULL,
		"expected redir out, got:\n%s", xr.value);
	FREE(xr.value);
	free_pipeline(&pr.value);
}

int	main(void)
{
	RUN_TEST(test_simple_command_xml);
	RUN_TEST(test_redirect_out_xml);
	RUN_TEST(test_redirect_in_xml);
	RUN_TEST(test_redirect_append_xml);
	RUN_TEST(test_pipe_xml);
	RUN_TEST(test_env_expansion_xml);
	RUN_TEST(test_multiple_redirects_xml);
	return (TEST_SUMMARY());
}
