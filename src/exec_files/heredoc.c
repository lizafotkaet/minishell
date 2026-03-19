#include "minishell.h"

static int	heredoc_read_loop(int fd, char *delimiter, char *tmpfile)
{
	char	*line;

	line = readline("> ");
	while (line && strcmp(line, delimiter) != 0)
	{
		write(fd, line, strlen(line));
		write(fd, "\n", 1);
		free(line);
		line = readline("> ");
		if (g_signal == SIGINT)
		{
			g_signal = 0;
			close(fd);
			unlink(tmpfile);
			return (-1);
		}
	}
	free(line);
	return (0);
}

char	*create_heredoc(char *delimiter)
{
	char	*tmpfile;
	int		fd;

	tmpfile = "/tmp/minishell_heredoc";
	fd = open(tmpfile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		perror("heredoc");
		return (NULL);
	}
	if (heredoc_read_loop(fd, delimiter, tmpfile) == -1)
		return (NULL);
	close(fd);
	return (tmpfile);
}