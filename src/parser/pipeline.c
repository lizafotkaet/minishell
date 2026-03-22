/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:58 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 19:27:10 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipeline.h"
#include "tokenize.h"
#include "libft.h"
#include "debug_alloc.h"

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
	pl->commands = ((t_command *)ft_calloc((
					pl->command_count), sizeof(t_command)));
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

t_result_t_pipeline	create_pipeline(const char *input, t_env *env)
{
	t_result_t_token_vector	tr;
	t_token_vector			tokens;
	t_pipeline				pl;

	if (input == NULL || *input == '\0')
		return (((t_result_t_pipeline){.is_error = true}));
	tr = tokenize_string(input);
	if (tr.is_error)
		return (((t_result_t_pipeline){.is_error = true}));
	tokens = tr.value;
	if (tokens.size == 0)
		return (m_token_vector_free(&tokens), ((
					t_result_t_pipeline){.is_error = true}));
	if (!substitute_all_envs(&tokens, *env))
		return (m_token_vector_free(&tokens), ((
					t_result_t_pipeline){.is_error = true}));
	ft_bzero(&pl, sizeof(t_pipeline));
	if (!build_all_commands(&tokens, &pl))
	{
		free_pipeline(&pl);
		m_token_vector_free(&tokens);
		return (((t_result_t_pipeline){.is_error = true}));
	}
	m_token_vector_free(&tokens);
	return (((t_result_t_pipeline){.is_error = false, .value = (pl)}));
}
