#include "pipeline_print.h"
#include "env.h"
#include "../libft/libft.h"
#include "debug_alloc.h"
#include <stdio.h>
#include <unistd.h>

static bool	read_input_line(char *line, size_t size)
{
	if (!isatty(STDIN_FILENO))
		return (fgets(line, size, stdin) != NULL);
	write(STDERR_FILENO, "minishell> ", 11);
	return (fgets(line, size, stdin) != NULL);
}

static void	strip_newline(char *s)
{
	size_t	len;

	len = ft_strlen(s);
	if (len > 0 && s[len - 1] == '\n')
		s[len - 1] = '\0';
}

static int	run_one(char *line, t_env env)
{
	RESULT(t_pipeline)	pr;
	RESULT(t_char_ptr)	xr;
	t_pipeline			pl;

	strip_newline(line);
	pr = create_pipeline(line, env);
	if (pr.is_error)
		return (ft_putstr_fd("error: parse failed\n", 2), 1);
	pl = pr.value;
	if (!read_heredocs(&pl))
	{
		free_pipeline(&pl);
		return (ft_putstr_fd("error: heredoc failed\n", 2), 1);
	}
	xr = pipeline_to_xml(&pl);
	free_pipeline(&pl);
	if (xr.is_error)
		return (ft_putstr_fd("error: xml failed\n", 2), 1);
	ft_putstr_fd(xr.value, 1);
	FREE(xr.value);
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	RESULT(t_env)	er;
	t_env			env;
	char			line[4096];

	(void)argc;
	(void)argv;
	er = parse_env(envp);
	if (er.is_error)
		return (1);
	env = er.value;
	while (read_input_line(line, sizeof(line)))
	{
		if (line[0] == '\n' || line[0] == '\0')
			continue ;
		run_one(line, env);
	}
	free_env(&env);
	return (0);
}
