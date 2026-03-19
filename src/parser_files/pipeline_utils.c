#include "../../include/pipeline.h"
#include "../../libft/libft.h"
#include "../../include/debug_alloc.h"

void	free_pipeline(t_pipeline *pipeline)
{
	int	i;

	if (pipeline == NULL)
		return ;
	if (pipeline->commands)
	{
		i = 0;
		while (i < pipeline->command_count)
		{
			free_command(&pipeline->commands[i]);
			i++;
		}
		FREE(pipeline->commands);
	}
}

void	free_command(t_command *cmd)
{
	int	i;

	if (cmd == NULL)
		return ;
	if (cmd->argv)
	{
		i = 0;
		while (i < cmd->argc)
		{
			FREE(cmd->argv[i]);
			i++;
		}
		FREE(cmd->argv);
	}
	if (cmd->redirects)
	{
		i = 0;
		while (i < cmd->redirect_count)
		{
			FREE(cmd->redirects[i].target);
			i++;
		}
		FREE(cmd->redirects);
	}
	m_string_vector_free(&cmd->heredoc_contents);
}
