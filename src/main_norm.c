// to compile: cc -lreadline main.c
 
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

int	g_signal = 0;

typedef struct s_cmd
{
	char			**argv;
	char			*infile;
	char			*outfile;
	char			*heredoc_tmpfile;
	int				append;
	int				is_builtin;
	struct s_cmd	*next;
}	t_cmd;

void	handle_sigint(int sig)
{
	g_signal = sig;
	write(1, "\n", 1);
}

void	setup_signals(void)
{
	struct sigaction	sa_int;
	struct sigaction	sa_quit;

	sa_int.sa_handler = handle_sigint;
	sigemptyset(&sa_int.sa_mask);
	sa_int.sa_flags = 0;
	sigaction(SIGINT, &sa_int, NULL);
	sa_quit.sa_handler = SIG_IGN;
	sigemptyset(&sa_quit.sa_mask);
	sa_quit.sa_flags = 0;
	sigaction(SIGQUIT, &sa_quit, NULL);
}

int	count_cmds(t_cmd *cmd)
{
	int	n;

	n = 0;
	while (cmd)
	{
		n++;
		cmd = cmd->next;
	}
	return (n);
}

static void	cleanup_pipes(int **pipes, int count)
{
	int	j;

	j = 0;
	while (j < count)
	{
		close(pipes[j][0]);
		close(pipes[j][1]);
		free(pipes[j]);
		j++;
	}
	free(pipes);
}

int	**alloc_pipes(int n_cmds)
{
	int		**pipes;
	int		i;

	if (n_cmds <= 1)
		return (NULL);
	pipes = malloc(sizeof(int *) * (n_cmds - 1));
	if (!pipes)
		return (NULL);
	i = 0;
	while (i < n_cmds - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		if (!pipes[i] || pipe(pipes[i]) == -1)
		{
			free(pipes[i]);
			cleanup_pipes(pipes, i);
			return (NULL);
		}
		i++;
	}
	return (pipes);
}

static void	exec_with_slash(char *cmd, char **argv, char **envp)
{
	if (access(cmd, F_OK) == -1)
	{
		perror(cmd);
		exit(127);
	}
	if (access(cmd, X_OK) == -1)
	{
		perror(cmd);
		exit(126);
	}
	execve(cmd, argv, envp);
	perror(cmd);
	exit(126);
}

static void	search_path(char *cmd, char **argv, char **envp, char *path)
{
	char	*dup;
	char	*dir;
	char	full[1024];

	dup = strdup(path);
	dir = strtok(dup, ":");
	while (dir)
	{
		snprintf(full, sizeof(full), "%s/%s", dir, cmd);
		execve(full, argv, envp);
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

void	exec_with_path(char *cmd, char **argv, char **envp)
{
	char	*path;

	if (strchr(cmd, '/'))
	{
		exec_with_slash(cmd, argv, envp);
		return ;
	}
	path = getenv("PATH");
	if (!path)
	{
		write(2, "command not found\n", 18);
		exit(127);
	}
	search_path(cmd, argv, envp, path);
}

int	is_builtin(char *cmd)
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

static int	builtin_echo(t_cmd *cmd)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	if (cmd->argv[1] && strcmp(cmd->argv[1], "-n") == 0)
	{
		newline = 0;
		i = 2;
	}
	while (cmd->argv[i])
	{
		printf("%s", cmd->argv[i]);
		if (cmd->argv[i + 1])
			printf(" ");
		i++;
	}
	if (newline)
		printf("\n");
	return (0);
}

static int	builtin_pwd(void)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)) != NULL)
	{
		printf("%s\n", cwd);
		return (0);
	}
	perror("pwd");
	return (1);
}

static int	builtin_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		printf("%s\n", envp[i]);
		i++;
	}
	return (0);
}

static int	builtin_cd(t_cmd *cmd)
{
	if (!cmd->argv[1] || cmd->argv[2])
	{
		write(2, "cd: wrong number of arguments\n", 30);
		return (1);
	}
	if (chdir(cmd->argv[1]) == -1)
	{
		perror("cd");
		return (1);
	}
	return (0);
}

static int	builtin_export(t_cmd *cmd)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		if (!strchr(cmd->argv[i], '='))
		{
			write(2, "export: invalid format\n", 23);
			return (1);
		}
		i++;
	}
	return (0);
}

static int	builtin_unset(t_cmd *cmd)
{
	if (!cmd->argv[1])
	{
		write(2, "unset: not enough arguments\n", 28);
		return (1);
	}
	return (0);
}

static int	builtin_exit(t_cmd *cmd)
{
	int	exit_code;

	exit_code = 0;
	if (cmd->argv[1])
		exit_code = atoi(cmd->argv[1]);
	exit(exit_code);
	return (0);
}

int	execute_builtin(t_cmd *cmd, char **envp)
{
	if (strcmp(cmd->argv[0], "echo") == 0)
		return (builtin_echo(cmd));
	if (strcmp(cmd->argv[0], "pwd") == 0)
		return (builtin_pwd());
	if (strcmp(cmd->argv[0], "env") == 0)
		return (builtin_env(envp));
	if (strcmp(cmd->argv[0], "cd") == 0)
		return (builtin_cd(cmd));
	if (strcmp(cmd->argv[0], "export") == 0)
		return (builtin_export(cmd));
	if (strcmp(cmd->argv[0], "unset") == 0)
		return (builtin_unset(cmd));
	if (strcmp(cmd->argv[0], "exit") == 0)
		return (builtin_exit(cmd));
	return (1);
}

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

static void	apply_input_redir(t_cmd *current, int i, int **pipes)
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

static void	apply_output_redir(t_cmd *current, int i, int n_cmds, int **pipes)
{
	if (current->outfile)
		open_and_redirect_out(current->outfile, current->append);
	else if (i < n_cmds - 1 && pipes)
		dup2(pipes[i][1], STDOUT_FILENO);
}

static void	close_all_pipes(int **pipes, int n_cmds)
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

static void	run_child_process(t_cmd *cur, int i, int n_cmds,
				int **pipes, char **envp)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	apply_input_redir(cur, i, pipes);
	apply_output_redir(cur, i, n_cmds, pipes);
	close_all_pipes(pipes, n_cmds);
	if (!cur->argv[0] || !cur->argv[0][0])
		exit(0);
	if (is_builtin(cur->argv[0]))
		exit(execute_builtin(cur, envp));
	else
		exec_with_path(cur->argv[0], cur->argv, envp);
}

static pid_t	fork_and_exec(t_cmd *current, int i, int n_cmds,
				int **pipes, char **envp)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
		run_child_process(current, i, n_cmds, pipes, envp);
	return (pid);
}

static pid_t	exec_one_cmd(t_cmd *cmd, int i, int n_cmds,
				int **pipes, char **envp)
{
	if (!cmd->argv || !cmd->argv[0])
		return (0);
	if (n_cmds == 1 && is_builtin(cmd->argv[0]))
		return (-2);
	return (fork_and_exec(cmd, i, n_cmds, pipes, envp));
}

static int	apply_builtin_input(t_cmd *cmd)
{
	int	fd;

	if (!cmd->infile)
		return (0);
	fd = open(cmd->infile, O_RDONLY);
	if (fd == -1)
	{
		perror(cmd->infile);
		return (-1);
	}
	dup2(fd, STDIN_FILENO);
	close(fd);
	return (0);
}

static int	apply_builtin_output(t_cmd *cmd)
{
	int	fd;

	if (!cmd->outfile)
		return (0);
	if (cmd->append)
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
	else
		fd = open(cmd->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		perror(cmd->outfile);
		return (-1);
	}
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (0);
}

static int	handle_parent_builtin(t_cmd *cmd, char **envp)
{
	int	saved_in;
	int	saved_out;
	int	status;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (apply_builtin_input(cmd) == -1 || apply_builtin_output(cmd) == -1)
	{
		dup2(saved_in, STDIN_FILENO);
		dup2(saved_out, STDOUT_FILENO);
		close(saved_in);
		close(saved_out);
		return (1);
	}
	status = execute_builtin(cmd, envp);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (status);
}

static void	close_parent_pipes(int **pipes, int count)
{
	int	i;

	if (!pipes)
		return ;
	i = 0;
	while (i < count)
	{
		close(pipes[i][0]);
		close(pipes[i][1]);
		i++;
	}
}

static void	free_pipes(int **pipes, int n_cmds)
{
	int	i;

	if (!pipes)
		return ;
	i = 0;
	while (i < n_cmds - 1)
	{
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

static void	cleanup_heredocs(t_cmd *cmd_list)
{
	t_cmd	*current;

	current = cmd_list;
	while (current)
	{
		if (current->heredoc_tmpfile)
			unlink(current->heredoc_tmpfile);
		current = current->next;
	}
}

static int	wait_all_children(pid_t last_pid, int forked_count)
{
	int	status;
	int	tmp;
	int	i;

	status = 0;
	if (forked_count == 0)
		return (0);
	if (last_pid != -1)
		waitpid(last_pid, &status, 0);
	i = 0;
	while (i < forked_count - 1)
	{
		waitpid(-1, &tmp, 0);
		i++;
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	return (1);
}

static int	pipeline_loop(t_cmd *cmd_list, int n_cmds, int **pipes, char **envp)
{
	t_cmd	*current;
	pid_t	last_pid;
	pid_t	pid;
	int		forked_count;
	int		i;

	i = 0;
	forked_count = 0;
	last_pid = -1;
	current = cmd_list;
	while (current)
	{
		pid = exec_one_cmd(current, i, n_cmds, pipes, envp);
		if (pid == -2)
			return (handle_parent_builtin(current, envp));
		if (pid == -1)
			return (1);
		forked_count += (pid > 0);
		if (pid > 0)
			last_pid = pid;
		current = current->next;
		i++;
	}
	close_parent_pipes(pipes, n_cmds - 1);
	return (wait_all_children(last_pid, forked_count));
}

int	run_pipeline(t_cmd *cmd_list, char **envp)
{
	int		n_cmds;
	int		**pipes;
	int		status;

	n_cmds = count_cmds(cmd_list);
	pipes = alloc_pipes(n_cmds);
	if (n_cmds > 1 && !pipes)
		return (1);
	status = pipeline_loop(cmd_list, n_cmds, pipes, envp);
	free_pipes(pipes, n_cmds);
	cleanup_heredocs(cmd_list);
	g_signal = 0;
	return (status);
}

static void	init_cmd_a(t_cmd *a, t_cmd *b, char **av)
{
	a->argv = av;
	a->next = b;
	a->is_builtin = 0;
	a->infile = NULL;
	a->outfile = NULL;
	a->heredoc_tmpfile = NULL;
	a->append = 0;
}

static void	init_cmd_b(t_cmd *b, char **av)
{
	b->argv = av;
	b->next = NULL;
	b->is_builtin = 0;
	b->infile = NULL;
	b->outfile = NULL;
	b->heredoc_tmpfile = NULL;
	b->append = 0;
}

int	main(int argc, char **argv, char **envp)
{
	int		last_exit_status;
	t_cmd	a;
	t_cmd	b;
	char	*av1[] = {"ls", NULL};
	char	*av2[] = {"wc", "-l", NULL};

	(void)argc;
	(void)argv;
	setup_signals();
	init_cmd_a(&a, &b, av1);
	init_cmd_b(&b, av2);
	last_exit_status = run_pipeline(&a, envp);
	printf("exit = %d\n", last_exit_status);
	return (0);
}

