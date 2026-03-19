#include "minishell.h"

/*
** Apply all redirects for a command in the original order.
** Heredoc (if any) is applied before explicit redirects.
** After all redirects, fall back to pipe if no explicit stdin/stdout.
*/
void	apply_all_redirs(t_cmd *current, int i, int n_cmds, int **pipes)
{
	int		j;
	int		fd;
	int		flags;
	int		has_in;
	int		has_out;

	has_in = 0;
	has_out = 0;
	if (current->heredoc_tmpfile)
	{
		fd = open(current->heredoc_tmpfile, O_RDONLY);
		if (fd == -1)
		{
			perror(current->heredoc_tmpfile);
			exit(1);
		}
		dup2(fd, STDIN_FILENO);
		close(fd);
		has_in = 1;
	}
	j = 0;
	while (j < current->n_redirs)
	{
		if (current->redirs[j].type == REDIR_IN)
		{
			fd = open(current->redirs[j].file, O_RDONLY);
			if (fd == -1)
			{
				perror(current->redirs[j].file);
				exit(1);
			}
			dup2(fd, STDIN_FILENO);
			close(fd);
			has_in = 1;
		}
		else
		{
			if (current->redirs[j].type == REDIR_APPEND)
				flags = O_WRONLY | O_CREAT | O_APPEND;
			else
				flags = O_WRONLY | O_CREAT | O_TRUNC;
			fd = open(current->redirs[j].file, flags, 0644);
			if (fd == -1)
			{
				perror(current->redirs[j].file);
				exit(1);
			}
			dup2(fd, STDOUT_FILENO);
			close(fd);
			has_out = 1;
		}
		j++;
	}
	if (!has_in && i > 0 && pipes)
		dup2(pipes[i - 1][0], STDIN_FILENO);
	if (!has_out && i < n_cmds - 1 && pipes)
		dup2(pipes[i][1], STDOUT_FILENO);
}

void	close_all_pipes(int **pipes, int n_cmds)
{
	int	j;

	if (!pipes)
		return ;
	j = 0;
	while (j < n_cmds - 1)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		j++;
	}
}
