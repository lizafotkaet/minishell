// to compile: cc -lreadline main.c

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#include <fcntl.h>  // For O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC, O_APPEND

#include <errno.h>
#include <signal.h>

#include <readline/readline.h>  // readline() for heredoc input prompt
#include <readline/history.h>   // add_history() for command history

// int g_last_exit_status = 0;
// only allowed global - stores signal number for signal handler
// must only store signal number, nothing else (see subject)
int	g_signal = 0;

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

// signal handler for SIGINT (ctrl-C)
void	handle_sigint(int sig)
{
	// store signal number in global (only legal use of global)
	g_signal = sig;
	write(1, "\n", 1);
}

// setup signal handlers for interactive mode
void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	// setup SIGINT handler (ctrl-C)
	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);

	// setup SIGQUIT handler (ctrl-\) - ignore in interactive mode
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

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

// // int **pipes → will hold pointers to arrays of two integers each (pipes[i][0] = read end, pipes[i][1] = write end).
int **alloc_pipes(int n_cmds)
{
	int i;
	int j;
	int **pipes;

	if (n_cmds <= 1)
		return (NULL);
	pipes = malloc(sizeof(int *) * (n_cmds - 1));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < n_cmds - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (!pipes[i])
		{
			// Cleanup: free all previously allocated pipes
			j = 0;
			while (j < i)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
				free(pipes[j]);
				j++;
			}
			free(pipes);
			return (NULL);
		}
		if (pipe(pipes[i]) == -1)
		{
			// Cleanup: free current pipe + all previous
			free(pipes[i]);
			j = 0;
			while (j < i)
			{
				close(pipes[j][0]);
				close(pipes[j][1]);
				free(pipes[j]);
				j++;
			}
			free(pipes);
			return (NULL);
		}
		i++;
	}
	return (pipes);
}


// execute a command with full $PATH search.
// Returns void because the function either replaces the process image via execve() or exits on error.
void	exec_with_path(char *cmd, char **argv, char **envp)
{
	char	*path;
	char	*dup;
	char	*dir;
	char	full[1024];

	if (strchr(cmd, '/'))
	{
		execve(cmd, argv, envp);
		// execve only returns on error
		perror(cmd);
		exit(126);  // Found but not executable
	}
	path = getenv("PATH");
	if (!path)
	{
		write(2, "command not found\n", 18);
		exit(127);
	}
	dup = strdup(path);
	dir = strtok(dup, ":");
	while (dir)
	{
		snprintf(full, sizeof(full), "%s/%s", dir, cmd);
		execve(full, argv, envp);

		// if execve failed due to permission, exit 126 (not 127)
		if (errno == EACCES)
		{
			free(dup);
			perror(cmd);
			exit(126);
		}

		dir = strtok(NULL, ":");
	}
	free(dup);
	write(2, "command not found\n", 18);
	exit(127);
}

int is_builtin(char *cmd)
{
	if (!cmd)
		return (0);
	if (strcmp(cmd, "echo") == 0)
		return (1);
	if (strcmp(cmd, "cd") == 0)
		return (1);
	if (strcmp(cmd, "pwd") == 0)
		return (1);
	if (strcmp(cmd, "export") == 0)
		return (1);
	if (strcmp(cmd, "unset") == 0)
		return (1);
	if (strcmp(cmd, "env") == 0)
		return (1);
	if (strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

int execute_builtin(t_cmd *cmd, char **envp)
{
	int		i;
	int		newline;
	char	cwd[1024];

	// handle echo builtin (prints arguments with optional -n flag)
	if (strcmp(cmd->argv[0], "echo") == 0)
	{
		i = 1;
		newline = 1;
		// check if -n flag is present (suppress trailing newline)
		if (cmd->argv[1] && strcmp(cmd->argv[1], "-n") == 0)
		{
			newline = 0;
			i = 2;
		}
		// print all arguments separated by spaces
		while (cmd->argv[i])
		{
			printf("%s", cmd->argv[i]);
			if (cmd->argv[i + 1])
				printf(" ");
			i++;
		}
		// print newline unless -n flag was used
		if (newline)
			printf("\n");
		return (0);
	}
	// handle pwd builtin (print current working directory)
	if (strcmp(cmd->argv[0], "pwd") == 0)
	{
		// getcwd fills buffer with current directory path
		if (getcwd(cwd, sizeof(cwd)) != NULL)
		{
			printf("%s\n", cwd);
			return (0);
		}
		perror("pwd");
		return (1);
	}
	// handle env builtin (print all environment variables)
	if (strcmp(cmd->argv[0], "env") == 0)
	{
		i = 0;
		// loop through environment array until NULL terminator
		while (envp[i])
		{
			printf("%s\n", envp[i]);
			i++;
		}
		return (0);
	}

		// handle cd builtin (change current working directory)
	if (strcmp(cmd->argv[0], "cd") == 0)
	{
		// cd requires exactly one argument (the target path)
		if (!cmd->argv[1] || cmd->argv[2])
		{
			write(2, "cd: wrong number of arguments\n", 30);
			return (1);
		}
		// chdir changes the process's current directory
		if (chdir(cmd->argv[1]) == -1)
		{
			perror("cd");
			return (1);
		}
		return (0);
	}

		// handle export builtin (add/modify environment variables)
	if (strcmp(cmd->argv[0], "export") == 0)
	{
		i = 1;
		// subject says no options, so just validate format
		// export requires arguments like VAR=value
		while (cmd->argv[i])
		{
			// check if argument contains '=' (valid export format)
			if (!strchr(cmd->argv[i], '='))
			{
				write(2, "export: invalid format\n", 23);
				return (1);
			}
			// note: actually modifying environment needs special handling
			// this validates syntax only for now
			i++;
		}
		return (0);
	}

	// handle unset builtin (remove environment variables)
	if (strcmp(cmd->argv[0], "unset") == 0)
	{
		// subject says no options
		// requires at least one variable name to unset
		if (!cmd->argv[1])
		{
			write(2, "unset: not enough arguments\n", 28);
			return (1);
		}
		// note: actually removing from environment needs special handling
		// this validates syntax only for now
		return (0);
	}

	if (strcmp(cmd->argv[0], "exit") == 0)
	{
		int	exit_code;
		
		exit_code = 0;
		// if argument provided, use it as exit code
		if (cmd->argv[1])
		{
			exit_code = atoi(cmd->argv[1]);
		}
		// exit terminates the entire process
		exit(exit_code);
	}
	return (1);
}


// reads input line by line until delimiter is found, writes to temp file
// called by parser when << is encountered, returns path to temp file
char	*create_heredoc(char *delimiter)
{
	char	*tmpfile;
	int		fd;
	char	*line;

	tmpfile = "/tmp/minishell_heredoc";
	// open temp file, create if not exists, erase previous content
	fd = open(tmpfile, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	if (fd == -1)
	{
		perror("heredoc");
		return (NULL);
	}
	// readline shows "> " prompt while reading heredoc input
	line = readline("> ");
	while (line && strcmp(line, delimiter) != 0)
	{
		// write line content and newline to temp file
		write(fd, line, strlen(line));
		write(fd, "\n", 1);
		free(line);
		line = readline("> ");
	}
	free(line);
	close(fd);
	return (tmpfile);
}

// Complete run_pipeline implementation

// Complete run_pipeline implementation
int run_pipeline(t_cmd *cmd_list, char **envp)
{
	int		n_cmds;        // Total number of commands in pipeline
	int		**pipes;       // Array of pipe pairs [read_fd, write_fd]
	int		i;             // Loop counter / command index
	pid_t	pid;           // Process ID from fork()
	pid_t	last_pid;      // PID of last command (for correct $? tracking)
	int		status;        // Exit status from wait()
	status = 0;            // initialize to 0 - prevents undefined behavior if no children ran

	t_cmd	*current;      // Iterator through linked list
	// int g_last_exit_status; // sorry i forgot this one

	n_cmds = count_cmds(cmd_list);
	// Count total commands (ls | wc = 2 commands)

	last_pid = -1;  // initialize to invalid pid

	pipes = alloc_pipes(n_cmds);
	// Allocate n_cmds-1 pipes (2 commands need 1 pipe)

	if (n_cmds > 1 && !pipes)
		return (1);
	// If multiple commands but pipe allocation failed, return error

	i = 0;
	current = cmd_list;
	// Initialize loop: start at command 0, point to first command

	while (current)
	{
		// skip commands with no arguments (empty input after parsing)
		if (!current->argv || !current->argv[0])
		{
			current = current->next;
			i++;
			continue;
		}

		// handle builtins that must run in parent (cd, export, unset, exit)
		// only if single command (no pipeline)
		if (n_cmds == 1 && is_builtin(current->argv[0]))
		{
			// handle redirections for builtin
			// then execute and return
			status = execute_builtin(current, envp);
			return (status);
		}
	
		pid = fork();
		// Create child process (child gets pid=0, parent gets child's PID)

		if (pid == -1)
		{
			perror("fork");
			return (1);
		}
		// Fork failed - print error and exit

		if (pid == 0)  // Child process executes this block
	{
	// restore default signal behavior in child
	// child must die normally on ctrl-C and ctrl-
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);

	// handle input redirection (heredoc takes priority over infile)
			// (heredoc input) ===
		if (current->heredoc_tmpfile)
		{
			int fd_in = open(current->heredoc_tmpfile, O_RDONLY);
			if (fd_in == -1)
			{
				perror(current->heredoc_tmpfile);
				exit(1);
			}
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}

		// regular < redirection, only if no heredoc (heredoc takes priority)
		else if (current->infile)
		{
			int fd_in = open(current->infile, O_RDONLY);
			if (fd_in == -1)
			{
				perror(current->infile);
				exit(1);
			}
			// redirect stdin to file, close original fd (stdin copy remains)
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}
		// no file redirection - read from previous command's pipe output
		// STDIN setup: redirect input from previous pipe
		else if (i > 0)  // Not first command (ls | wc: wc needs input from pipe)
		{
			dup2(pipes[i - 1][0], STDIN_FILENO);
			// Replace stdin with read-end of previous pipe
			// pipes[i-1][0] = read end of pipe BEFORE this command
		}

		// STDOUT setup: redirect output to next pipe
		
		// handle output file redirection (> and >>)
		if (current->outfile)
		{
			int fd_out;
			// append mode (>>) keeps existing content, trunc (>) erases it
			if (current->append)
				fd_out = open(current->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
			else
				fd_out = open(current->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd_out == -1)
			{
				perror(current->outfile);
				exit(1);
			}
			// redirect stdout to file, close original fd (stdout copy remains)
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}
		// no file redirection - write to next command's pipe input
		else if (i < n_cmds - 1)
		{
			dup2(pipes[i][1], STDOUT_FILENO);
			// pipes[i][1] = write end of pipe AFTER this command
		}

		// Close ALL pipe FDs in child (no longer needed after dup2)
		int j = 0;
		while (j < n_cmds - 1)
		{
			close(pipes[j][0]);  // Close read end
			close(pipes[j][1]);  // Close write end
			j++;
		}
		// Why? After dup2, original pipe FDs are duplicated to 0/1
		// Leaving them open wastes FDs and can cause hanging pipes

		// change this line - check if builtin first
		if (is_builtin(current->argv[0]))
			exit(execute_builtin(current, envp));
		else
			exec_with_path(current->argv[0], current->argv, envp);
			// Why? After dup2, original pipe FDs are duplicated to 0/1
			// Leaving them open wastes FDs and can cause hanging pipes
			// Replace child process with command (never returns on success)
	}
		// Parent continues here (pid != 0)
		// track pid of last command for accurate $? exit status
		if (i == n_cmds - 1)
			last_pid = pid;
			
		current = current->next;
		i++;
		// Move to next command in linked list
	}

	// Parent: close all pipes (children have their own copies)
	i = 0;
	while (i < n_cmds - 1)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
	// Why? Parent doesn't need pipes. Closing prevents orphaned FDs

	// Wait for all children to finish
	i = 0;

	// problem was: collects any child in any order. status after the loop may not be from the last command.
	// while (i < n_cmds)
	// {
	// 	wait(&status);  // Blocks until any child exits
	// 	i++;
	// }

	// wait for all children, but track last command's exit status specifically
	i = 0;

	// wait for last command first to capture its exit status for $?
	if (last_pid != -1)
		waitpid(last_pid, &status, 0);
	// then wait for remaining children (order doesn't matter for them)


		i = 0;
	while (i < n_cmds - 1)  // n_cmds - 1 because we already waited for last_pid
	{
		// waitpid(-1) waits for any child, same as wait()
		// but lets us check which pid just exited
		waitpid(-1, &status, 0);
		i++;
	}

	// Note: This waits for ANY child, not in order
	// Last wait() will capture last command's exit status

	// Free allocated pipe memory
	if (pipes)
	{
		i = 0;
		while (i < n_cmds - 1)
		{
			free(pipes[i]);
			i++;
		}
		free(pipes);
	}

	// Return exit code of last command and save for $? expansion
	if (WIFEXITED(status))
	{
		// child exited normally - save and return actual exit code
		// g_last_exit_status = WEXITSTATUS(status);
		return (WEXITSTATUS(status));
	}
	// WIFEXITED checks if child exited normally
	// WEXITSTATUS extracts actual exit code (0-255)

	// child died from signal - save and return error code
	// g_last_exit_status = 1;
	return (1);
	// If child didn't exit normally (signal), return error

}

int main(int argc, char **argv, char **envp)
{
	int		last_exit_status;
	(void)argc;
	(void)argv;

	// tracks exit status of last command for $? expansion
	// passed to parser so it can replace $? with this value
	last_exit_status = 0;

	t_cmd a, b;

	// initialize signal handlers before anything else
	setup_signals();

	char *av1[] = {"ls", NULL};
	char *av2[] = {"wc", "-l", NULL};

	a.argv = av1;
	a.next = &b;
	a.is_builtin = 0;

	// fix for file not found
	a.infile = NULL;
	a.outfile = NULL;
	a.heredoc_tmpfile = NULL;
	a.append = 0;

	b.argv = av2;
	b.next = NULL;
	b.is_builtin = 0;
	// fix for file not found
	b.infile = NULL;
	b.outfile = NULL;
	b.heredoc_tmpfile = NULL;
	b.append = 0;

	last_exit_status = run_pipeline(&a, envp);

	printf("exit = %d\n", last_exit_status);


	// voorlopige example readline loop for parser integration:
	// while (1)
	// {
	//     char *line = readline("minishell$ ");
	//     if (!line)  // ctrl-D pressed (EOF)
	//         break;
	//     if (*line)  // non-empty line
	//         add_history(line);
	//     // parser converts line to t_cmd list
	//     // last_exit_status = run_pipeline(cmd_list, envp);
	//     free(line);
	// }
	// printf("exit\n");
}

// test new gadget connection