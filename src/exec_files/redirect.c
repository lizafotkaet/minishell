#include "../minishell.h"

static void	open_and_redirect_in(char *file)
{
	int	fd;

	fd = open(file, O_RDONLY);
	if (fd == -1)
	{
		perror(file);
		exit(1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
}

void	apply_input_redir(t_cmd *current, int i, int **pipes)
{
	if (current->heredoc_tmpfile)
		open_and_redirect_in(current->heredoc_tmpfile);
	else if (current->infile)
		open_and_redirect_in(current->infile);
	else if (i > 0 && pipes)
		dup2(pipes[i - 1][0], STDIN_FILENO);
}

static void	open_and_redirect_out(char *file, int append)
{
	int	fd;

	if (append)
		fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror(file);
		exit(1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
}

void	apply_output_redir(t_cmd *current, int i, int n_cmds, int **pipes)
{
	if (current->outfile)
		open_and_redirect_out(current->outfile, current->append);
	else if (i < n_cmds - 1 && pipes)
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