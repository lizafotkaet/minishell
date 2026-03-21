/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   buffer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:01:33 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 04:19:08 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "buffer.h"
#include "debug_alloc.h"

/*
** Initialise a new t_buffer with BUFFER_INITIAL_CAPACITY.
** The buffer is always null-terminated: data[0] == '\0' after creation.
** Returns SUCCESS with the buffer, or ERROR on allocation failure.
*/

RESULT(t_buffer)	m_buffer_new(void)
{
	t_buffer	buf;

	buf.data = ALLOC(char, BUFFER_INITIAL_CAPACITY);
	if (!buf.data)
		return (ERROR(t_buffer));
	buf.data[0] = '\0';
	buf.size = 0;
	buf.capacity = BUFFER_INITIAL_CAPACITY;
	return (SUCCESS(t_buffer, buf));
}

/*
** Ensure that `buf` can hold at least `required` total bytes.
** Uses a x2 reallocation strategy: doubles capacity until it is
** large enough, then copies existing content into the new allocation.
** Returns true on success, false on malloc failure (buffer unchanged).
*/

static bool	buffer_grow(t_buffer *buf, size_t required)
{
	size_t	new_cap;
	char	*new_data;

	if (required <= buf->capacity)
		return (true);
	new_cap = buf->capacity;
	while (new_cap < required)
		new_cap *= 2;
	new_data = ALLOC(char, new_cap);
	if (!new_data)
		return (false);
	ft_memcpy(new_data, buf->data, buf->size);
	new_data[buf->size] = '\0';
	FREE(buf->data);
	buf->data = new_data;
	buf->capacity = new_cap;
	return (true);
}

/*
** Append `n` bytes from `src` into the buffer.
** The buffer is null-terminated after the write.
** Returns true on success, false on allocation failure.
*/

bool	m_buffer_write(t_buffer *buf, const char *src, size_t n)
{
	if (n == 0)
		return (true);
	if (!buffer_grow(buf, buf->size + n + 1))
		return (false);
	ft_memcpy(buf->data + buf->size, src, n);
	buf->size += n;
	buf->data[buf->size] = '\0';
	return (true);
}

/*
** Return a heap-allocated copy of the null-terminated buffer contents.
** The caller is responsible for freeing the returned string.
** Returns NULL on allocation failure.
*/

char	*m_buffer_read(t_buffer *buf)
{
	char	*copy;

	copy = ft_strdup(buf->data);
	return (copy);
}

/*
** Free the buffer's underlying data.
*/

void	m_buffer_free(t_buffer *buf)
{
	if (!buf || !buf->data)
		return ;
	FREE(buf->data);
	buf->data = NULL;
	buf->size = 0;
	buf->capacity = 0;
}
