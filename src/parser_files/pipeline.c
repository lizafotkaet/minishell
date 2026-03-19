#include "../../include/pipeline.h"
#include "../../include/tokenize.h"
#include "../../libft/libft.h"
#include "../../include/debug_alloc.h"


/*
** Count how many pipe tokens are in the vector.
*/
static int	count_pipes(t_token_vector tokens)
{
	size_t	i;
	int		n;

	n = 0;
	i = 0;
	while (i < tokens.size)
	{
		if (tokens.data[i].token_type == E_PIPE)
			n++;
		i++;
	}
	return (n);
}


/*
** Build all commands from the token vector, splitting by pipes.
*/
static bool	build_all_commands(t_token_vector *tokens, t_pipeline *pl)
{
	size_t	i;
	size_t	seg_start;
	int		ci;

	pl->command_count = count_pipes(*tokens) + 1;
	pl->commands = ALLOC(t_command, pl->command_count);
	if (!pl->commands)
		return (false);
	seg_start = 0;
	ci = 0;
	i = 0;
	while (i <= tokens->size)
	{
		if (i == tokens->size
			|| tokens->data[i].token_type == E_PIPE)
		{
			if (!build_command(tokens, seg_start, i, &pl->commands[ci]))
				return (false);
			ci++;
			seg_start = i + 1;
		}
		i++;
	}
	return (true);
}


/*
** Create a pipeline from a raw input string.
**
** Steps:
**   1) Tokenize into a t_token_vector.
**   2) Substitute env vars in word tokens only.
**   3) Split by pipes, build each command.
**   5) Validate: no empty commands, redirect always followed by word,
**      each command has at least one word (argv[0]).
**
** Returns RESULT(t_pipeline): SUCCESS with the pipeline,
** or ERROR on syntax error or allocation failure.
*/
RESULT(t_pipeline)	create_pipeline(const char *input, t_env env)
{
	RESULT(t_token_vector)	tr;
	t_token_vector			tokens;
	t_pipeline				pl;

	if (input == NULL || *input == '\0')
		return (ERROR(t_pipeline));
	tr = tokenize_string(input);
	if (tr.is_error)
		return (ERROR(t_pipeline));
	tokens = tr.value;
	if (tokens.size == 0)
		return (m_token_vector_free(&tokens), ERROR(t_pipeline));
	if (!substitute_all_envs(&tokens, env))
		return (m_token_vector_free(&tokens), ERROR(t_pipeline));
	ft_bzero(&pl, sizeof(t_pipeline));
	if (!build_all_commands(&tokens, &pl))
	{
		free_pipeline(&pl);
		m_token_vector_free(&tokens);
		return (ERROR(t_pipeline));
	}
	m_token_vector_free(&tokens);
	return (SUCCESS(t_pipeline, pl));
}
