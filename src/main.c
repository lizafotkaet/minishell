#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

// Placeholder struct-ure for now ()
typedef struct s_cmd
{
	char            **argv;
	char            *infile;
	char            *outfile;
	char            *heredoc_tmpfile;
	int             append;
	int             is_builtin;
	struct s_cmd    *next;
}   t_cmd;

// check
int count_cmds(t_cmd *cmd)
{
	int n = 0;
	while (cmd)
	{
		n++;
		cmd = cmd->next;
	}
	return (n);
}

// int **pipes → will hold pointers to arrays of two integers each (pipes[i][0] = read end, pipes[i][1] = write end).
int **alloc_pipes(int n_cmds)
{
	int i;
	int **pipes;
	// int **pipes → will hold pointers to arrays of two integers each (pipes[i][0] = read end, pipes[i][1] = write end).

	if (n_cmds <= 1)
	return NULL;
	// if 1 or less cmds no pipes needed, exit

	pipes = malloc(sizeof(int *) * (n_cmds - 1));
	if (!pipes)
	return NULL;
	// check malloc success

	i = 0;

	while (i < n_cmds - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (!pipes[i])
			return (NULL);
		// create unidirectional pipe
		if (pipe(pipes[i]) == -1)
			return (NULL);
		i++;
	}
	return (pipes);
}

// execute a command with full $PATH search.
// Returns void because the function either replaces the process image via execve() or exits on error.
void	exec_with_path(char *cmd, char **argv, char **envp)
{
	char    *path;
	char    *dup;
	char    *dir;
	char    full[1024];
	// full[1024] → buffer to construct the full path for execve (directory + "/" + command).

	if (strchr(cmd, '/'))
    	execve(cmd, argv, envp);

	path = getenv("PATH");

	if (!path)
	{
		write(2, "command not found\n", 18);
		exit(127);
		// In shells, exit code 127 is a convention meaning “command not found”.
		// 1 - 125 normal exits
		// 126 = found but not executable = permission denied
	}

	dup = strdup(path);
	//strdup(path) creates a heap-allocated copy of the PATH. so strtok() doesn't modify the original string
	dir = strtok(dup, ":");
	// strtok() splits a string into tokens based on delimiters (: here).
	// A token is simply a piece of a string separated by a delimiter.

	while (dir)
	{
		snprintf(full, sizeof(full), "%s/%s", dir, cmd);
		execve(full, argv, envp);
		dir = strtok(NULL, ":");
		// Passing NULL tells it: “continue tokenizing the same string from where you stopped.”
	}
	free(dup);
	// (on a successful execve(), the entire process memory is overwritten, including all heap allocations.)

	write(2, "command not found\n", 18);
	exit(127);
}

int run_pipeline(t_cmd *cmd_list, char **envp)
{

}

int main(int argc, char **argv, char **envp)
{
	(void)argc;
	(void)argv;

	t_cmd a, b;

	char *av1[] = {"ls", NULL};
	char *av2[] = {"wc", "-l", NULL};

	a.argv = av1;
	a.next = &b;
	a.is_builtin = 0;

	b.argv = av2;
	b.next = NULL;
	b.is_builtin = 0;

	int status = run_pipeline(&a, envp);
	printf("exit = %d\n", status);
}
