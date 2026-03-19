#include "minishell.h"
#include "pipeline.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
 
/*
** Converts the parser's t_pipeline into the executor's t_cmd linked list.
** The two structs represent the same data differently:
**   t_pipeline  → array of t_command (parser output)
**   t_cmd       → linked list         (executor input)
*/
static t_cmd	*pipeline_to_cmd_list(t_pipeline *pl)
{
	t_cmd		*head;
	t_cmd		*node;
	t_command	*cmd;
	int			i;
	int			j;
 
	head = NULL;
	i = pl->command_count - 1;
	while (i >= 0)
	{
		node = (t_cmd *)calloc(1, sizeof(t_cmd));
		if (!node)
		{
			while (head)
			{
				node = head->next;
				free(head);
				head = node;
			}
			return (NULL);
		}
		cmd = &pl->commands[i];
		node->argv = cmd->argv;
		node->is_builtin = 0;
		node->infile = NULL;
		node->outfile = NULL;
		node->heredoc_tmpfile = NULL;
		node->append = 0;
		j = 0;
		while (j < cmd->redirect_count)
		{
			if (cmd->redirects[j].type == E_REDIR_IN)
				node->infile = cmd->redirects[j].target;
			else if (cmd->redirects[j].type == E_REDIR_OUT)
				node->outfile = cmd->redirects[j].target;
			else if (cmd->redirects[j].type == E_REDIR_APPEND)
			{
				node->outfile = cmd->redirects[j].target;
				node->append = 1;
			}
			else if (cmd->redirects[j].type == E_HEREDOC)
			{
				if (cmd->heredoc_contents.size > 0)
					node->heredoc_tmpfile =
						(char *)cmd->heredoc_contents.data[0];
			}
			j++;
		}
		node->next = head;
		head = node;
		i--;
	}
	return (head);
}
 
static void	free_cmd_list(t_cmd *head)
{
	t_cmd	*next;
 
	while (head)
	{
		next = head->next;
		free(head);
		head = next;
	}
}
 
int	main(int argc, char **argv, char **envp)
{
	char				*line;
	t_result_t_env		env_res;
	t_env				env;
	t_result_t_pipeline	pl_res;
	t_pipeline			pl;
	t_cmd				*cmd_list;
	int					last_exit;
 
	(void)argc;
	(void)argv;
	env_res = parse_env(envp);
	if (env_res.is_error)
		return (1);
	env = env_res.value;
	last_exit = 0;
	setup_signals();
	while (1)
	{
		line = readline("minishell$ ");
		if (!line)
			break ;
		if (*line)
			add_history(line);
		pl_res = create_pipeline(line, env);
		free(line);
		if (pl_res.is_error)
			continue ;
		pl = pl_res.value;
		cmd_list = pipeline_to_cmd_list(&pl);
		if (cmd_list)
		{
			last_exit = run_pipeline(cmd_list, m_env_to_char_ptr(env));
			m_env_set_exit_code(&env, last_exit);
			free_cmd_list(cmd_list);
		}
		free_pipeline(&pl);
	}
	free_env(&env);
	return (last_exit);
}
