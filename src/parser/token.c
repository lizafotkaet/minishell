/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:03:24 by liza              #+#    #+#             */
/*   Updated: 2026/03/19 13:03:26 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "token.h"
#include "libft.h"
#include "debug_alloc.h"

/*
** Free the content of a single token if it was allocated.
*/
void	m_token_free(t_token *token)
{
	if (token == NULL)
		return ;
	if (token->allocated && token->content != NULL)
	{
		FREE(token->content);
		token->content = NULL;
	}
}

/*
** Initialise a new t_token_vector with TOKEN_VECTOR_INITIAL_CAPACITY.
** Returns SUCCESS with the vector, or ERROR on allocation failure.
*/
RESULT(t_token_vector)	m_token_vector_new(void)
{
	t_token_vector	vec;

	vec.data = ((t_token *)ft_calloc((TOKEN_VECTOR_INITIAL_CAPACITY), sizeof(t_token)));
	if (!vec.data)
		return (((t_result_t_token_vector){.is_error = true}));
	vec.size = 0;
	vec.capacity = TOKEN_VECTOR_INITIAL_CAPACITY;
	return (((t_result_t_token_vector){.is_error = false, .value = (vec)}));
}

/*
** Ensure that `vec` can hold at least `required` tokens.
** Uses a x2 reallocation strategy.
** Returns true on success, false on malloc failure.
*/
static bool	token_vector_grow(t_token_vector *vec, size_t required)
{
	size_t	new_cap;
	t_token	*new_data;

	if (required < vec->capacity)
		return (true);
	new_cap = vec->capacity;
	while (new_cap <= required)
		new_cap *= 2;
	new_data = ((t_token *)ft_calloc((new_cap), sizeof(t_token)));
	if (!new_data)
		return (false);
	ft_memcpy(new_data, vec->data, vec->size * sizeof(t_token));
	FREE(vec->data);
	vec->data = new_data;
	vec->capacity = new_cap;
	return (true);
}

/*
** Append a token to the vector.
** The token is copied by value; the caller keeps ownership of any
** heap-allocated content (the vector does NOT duplicate the string).
** Returns true on success, false on allocation failure.
*/
bool	m_token_vector_append(t_token_vector *vec, t_token tok)
{
	if (!token_vector_grow(vec, vec->size + 1))
		return (false);
	vec->data[vec->size] = tok;
	vec->size++;
	return (true);
}

/*
** Free all tokens whose `allocated` flag is set, and the data array.
*/
void	m_token_vector_free(t_token_vector *vec)
{
	size_t	i;

	if (!vec || !vec->data)
		return ;
	i = 0;
	while (i < vec->size)
	{
		m_token_free(&vec->data[i]);
		i++;
	}
	FREE(vec->data);
	vec->data = NULL;
	vec->size = 0;
	vec->capacity = 0;
}
