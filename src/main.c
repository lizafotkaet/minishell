/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 20:52:17 by ebarbash          #+#    #+#             */
/*   Updated: 2026/03/22 20:52:18 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/minishell.h"
#include "pipeline.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>

void	free_cmd_list(t_cmd *head)        /* no longer static */
{
	t_cmd	*next;

	while (head)
	{
		next = head->next;
		free(head->redirs);
		free(head);
		head = next;
	}
}

static int	process_line(char *line, t_env *env)
{
	t_result_t_pipeline	pl_res;
	t_pipeline			pl;
	t_cmd				*cmd_list;
	int					last_exit;

	pl_res = create_pipeline(line, env); // *env -> env
	if (pl_res.is_error)
		return (-1);
	pl = pl_res.value;
	if (!read_heredocs(&pl))
	{
		free_pipeline(&pl);
		return (-1);
	}
	last_exit = -1;
	cmd_list = pipeline_to_cmd_list(&pl);
	if (cmd_list)
	{
		last_exit = run_pipeline(cmd_list, env);
		m_env_set_exit_code(env, last_exit);
		free_cmd_list(cmd_list);
	}
	free_pipeline(&pl);
	return (last_exit);
}

static int	shell_loop(t_env *env)
{
	char	*line;
	int		last_exit;
	int		ret;

	last_exit = 0;
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
			break ;
		if (*line)
			add_history(line);
		ret = process_line(line, env);
		free(line);
		if (ret >= 0)
			last_exit = ret;
	}
	return (last_exit);
}

int	main(int argc, char **argv, char **envp)
{
	t_result_t_env	env_res;
	t_env			env;
	int				last_exit;

	(void)argc;
	(void)argv;
	if (argc != 1)
		return (-1);
	env_res = parse_env(envp);
	if (env_res.is_error)
		return (1);
	env = env_res.value;
	setup_signals();
	last_exit = shell_loop(&env);
	free_env(&env);
	return (last_exit);
}
