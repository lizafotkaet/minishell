/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_path.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:14:14 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/21 17:14:27 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
