#ifndef COMMAND_H
# define COMMAND_H

# include "token.h"
# include "string_vector.h"

typedef struct s_redirect
{
	t_token_type	type;
	char			*target;
}	t_redirect;

typedef struct s_command
{
	char			**argv;
	int				argc;
	t_redirect		*redirects;
	int				redirect_count;
	t_string_vector	heredoc_contents;
}	t_command;

bool	build_command(t_token_vector *tokens, size_t start,
			size_t end, t_command *cmd);
void	free_command(t_command *cmd);

#endif
