/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   substitute_env_internal.h                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:17:54 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 19:56:40 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SUBSTITUTE_ENV_INTERNAL_H
# define SUBSTITUTE_ENV_INTERNAL_H

# include "env.h"
# include "buffer.h"

const char					*expand_variable(
								const char *it, t_env env, t_buffer *buf);
const char					*expand_char_within_double_quotes(const char *it,
								const char *end, t_env env, t_buffer *buf);
t_result_t_const_char_ptr	substitute_env_with_buffer(const char *it,
								t_env env, t_buffer *buf);
t_result_t_const_char_ptr	process_next_segment(const char *it,
								t_env env, t_buffer *buf);
t_result_t_const_char_ptr	process_unquoted_char(const char *it,
								t_env env, t_buffer *buf);
t_result_t_const_char_ptr	process_double_quoted(const char *it,
								t_env env, t_buffer *buf);
t_result_t_const_char_ptr	process_single_quoted(const char *it,
								t_buffer *buf);

#endif
