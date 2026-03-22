/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenize.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:18:18 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 18:46:04 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TOKENIZE_H
# define TOKENIZE_H

# include <stdbool.h>
# include "result.h"
# include "quotes.h"
# include "token.h"

bool						is_whitespace(char c);
const char					*skip_consecutive_whitespaces(const char *s);
bool						is_operator_char(char c);
int							operator_length(const char *s);
t_token_type				operator_type(const char *s, int len);
t_result_t_const_char_ptr	skip_quoted_segment(const char *it);
t_result_t_const_char_ptr	skip_word(const char *it);
t_result_t_token_vector		tokenize_string(const char *input);

#endif
