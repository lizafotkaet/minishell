/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:17:38 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:48:06 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef QUOTES_H
# define QUOTES_H

# include <stdbool.h>
# include "result.h"

/* Declare result type for char pointers (const char *) */
typedef const char	*t_const_char_ptr;
typedef struct s_result_t_const_char_ptr
{
	bool				is_error;
	t_const_char_ptr	value;
}	t_result_t_const_char_ptr;

typedef char		*t_char_ptr;
typedef struct s_result_t_char_ptr
{
	bool				is_error;
	t_char_ptr			value;
}	t_result_t_char_ptr;

bool						character_is_escaped(
								const char *start, const char *pos);
t_result_t_const_char_ptr	closing_double_quote_position(
								const char *quote_position);
t_result_t_const_char_ptr	closing_single_quote_position(
								const char *quote_position);

#endif
