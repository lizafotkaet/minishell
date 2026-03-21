/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:16:43 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 15:21:44 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <unistd.h>
# include <stdlib.h>
# include <stdio.h>
# include <sys/wait.h>
# include <string.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <readline/readline.h>
# include <readline/history.h>
#include <signal.h>
#include "command.h"
#include "pipeline.h"
#include "minishell_p.h"

extern int	g_signal;

# define REDIR_IN     0
# define REDIR_OUT    1
# define REDIR_APPEND 2

typedef struct s_redir_exec
{
	char	*file;
	int		type;
}	t_redir_exec;

typedef struct s_cmd
{
	char			**argv;
	char			*heredoc_tmpfile;
	t_redir_exec	*redirs;
	int				n_redirs;
	int				is_builtin;
	struct s_cmd	*next;
}	t_cmd;

typedef struct s_exec_ctx
{
	int		i;
	int		n_cmds;
	int		**pipes;
}	t_exec_ctx;

// signals.c
void	handle_sigint(int sig);
void	setup_signals(void);
// pipes.c
int		count_cmds(t_cmd *cmd);
int		**alloc_pipes(int n_cmds);
void	close_parent_pipes(int **pipes, int count);
void	free_pipes(int **pipes, int n_cmds);
// exec_path.c
void	exec_with_path(char *cmd, char **argv, char **envp);
// builtins.c
int		is_builtin(char *cmd);
int		execute_builtin(t_cmd *cmd, char **envp);
// builtins_2.c
int		builtin_echo(t_cmd *cmd);
int		builtin_pwd(void);
int		builtin_env(char **envp);
int		builtin_cd(t_cmd *cmd);
// heredoc.c
char	*create_heredoc(char *delimiter);
// redirect.c
void	apply_all_redirs(t_cmd *current, int i, int n_cmds, int **pipes);
void	close_all_pipes(int **pipes, int n_cmds);
//redirect_builtin.c
int		handle_parent_builtin(t_cmd *cmd, char **envp);
// exec.c
pid_t	exec_one_cmd(t_cmd *cmd, t_exec_ctx *ctx, char **envp);
// pipeline.c
int		run_pipeline(t_cmd *cmd_list, char **envp);
// cmd_convert.c
t_cmd	*pipeline_to_cmd_list(t_pipeline *pl);
// main.c
void	free_cmd_list(t_cmd *head);

#endif
