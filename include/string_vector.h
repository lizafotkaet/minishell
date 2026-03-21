/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string_vector.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:17:49 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 04:17:50 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRING_VECTOR_H
# define STRING_VECTOR_H

# include <stdbool.h>
# include <stddef.h>
# include "result.h"

# define STRING_VECTOR_INITIAL_CAPACITY 32

typedef struct s_string_vector
{
	char	**data;
	size_t	size;
	size_t	capacity;
}	t_string_vector;

DECLARE_RESULT(t_string_vector);

RESULT(t_string_vector)	m_string_vector_new(void);
bool					m_string_vector_append(
						t_string_vector *vec, const char *s);
const char				*m_string_vector_at(
						t_string_vector *vec, size_t index);
void					m_string_vector_free(
						t_string_vector *vec);

#endif
