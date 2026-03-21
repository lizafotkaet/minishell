/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   substitute_env_internal.h                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:17:54 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 04:17:55 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SUBSTITUTE_ENV_INTERNAL_H
# define SUBSTITUTE_ENV_INTERNAL_H

# include "env.h"
# include "buffer.h"

const char	*expand_variable(const char *it, t_env env, t_buffer *buf);
const char	*expand_char_within_double_quotes(const char *it,
				const char *end, t_env env, t_buffer *buf);

#endif
