/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 10:02:21 by liza              #+#    #+#             */
/*   Updated: 2026/03/20 10:02:24 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUFFER_H
# define BUFFER_H

# include <stdbool.h>
# include <stddef.h>
# include "result.h"

# define BUFFER_INITIAL_CAPACITY 128

typedef struct s_buffer
{
	char	*data;
	size_t	size;
	size_t	capacity;
}	t_buffer;

DECLARE_RESULT(t_buffer);

RESULT(t_buffer)	m_buffer_new(void);
bool				m_buffer_write(t_buffer *buf, const char *src,
					size_t n);
char				*m_buffer_read(t_buffer *buf);
void				m_buffer_free(t_buffer *buf);

#endif
