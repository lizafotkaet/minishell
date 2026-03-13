#ifndef TOKEN_H
# define TOKEN_H

# include <stdbool.h>
# include <stddef.h>
# include "result.h"


typedef enum e_token_type
{
	E_WORD,
	E_PIPE,
	E_HEREDOC,
	E_REDIR_APPEND,
	E_REDIR_OUT,
	E_REDIR_IN
}	t_token_type;


typedef struct s_token
{
	t_token_type	token_type;
	char			*content;
	bool			allocated;
}	t_token;

void	m_token_free(t_token *token);


# define TOKEN_VECTOR_INITIAL_CAPACITY 32

typedef struct s_token_vector
{
	t_token	*data;
	size_t	size;
	size_t	capacity;
}	t_token_vector;

DECLARE_RESULT(t_token_vector);

RESULT(t_token_vector)	m_token_vector_new(void);
bool					m_token_vector_append(t_token_vector *vec,
							t_token tok);
void					m_token_vector_free(t_token_vector *vec);

#endif
