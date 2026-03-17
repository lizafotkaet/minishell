#include "minishell.h"

static void	init_cmd_a(t_cmd *a, t_cmd *b, char **av)
{
	a->argv = av;
	a->next = b;
	a->is_builtin = 0;
	a->infile = NULL;
	a->outfile = NULL;
	a->heredoc_tmpfile = NULL;
	a->append = 0;
}

static void	init_cmd_b(t_cmd *b, char **av)
{
	b->argv = av;
	b->next = NULL;
	b->is_builtin = 0;
	b->infile = NULL;
	b->outfile = NULL;
	b->heredoc_tmpfile = NULL;
	b->append = 0;
}

int	main(int argc, char **argv, char **envp)
{
	int		last_exit_status;
	t_cmd	a;
	t_cmd	b;
	char	*av1[] = {"ls", NULL};
	char	*av2[] = {"wc", "-l", NULL};

	(void)argc;
	(void)argv;
	setup_signals();
	init_cmd_a(&a, &b, av1);
	init_cmd_b(&b, av2);
	last_exit_status = run_pipeline(&a, envp);
	printf("exit = %d\n", last_exit_status);
	return (0);
}