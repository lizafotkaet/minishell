#include "string_vector.h"
#include "../libft/libft.h"
#include "debug_alloc.h"

/*
** Allocate a new t_string_vector with STRING_VECTOR_INITIAL_CAPACITY.
** The data array is null-terminated: data[0] == NULL after creation.
** Returns SUCCESS with the vector, or ERROR on allocation failure.
*/
RESULT(t_string_vector)	m_string_vector_new(void)
{
	t_string_vector	vec;

	vec.data = ALLOC(char *, STRING_VECTOR_INITIAL_CAPACITY);
	if (!vec.data)
		return (ERROR(t_string_vector));
	vec.size = 0;
	vec.capacity = STRING_VECTOR_INITIAL_CAPACITY;
	return (SUCCESS(t_string_vector, vec));
}

/*
** Ensure that `vec` can hold at least `required` string pointers
** (not counting the NULL terminator slot).
** Uses a x2 reallocation strategy: doubles capacity until it is
** large enough, then copies existing pointers into the new allocation.
** Returns true on success, false on malloc failure (vector unchanged).
*/
static bool	vector_grow(t_string_vector *vec, size_t required)
{
	size_t	new_cap;
	char	**new_data;

	if (required < vec->capacity)
		return (true);
	new_cap = vec->capacity;
	while (new_cap <= required)
		new_cap *= 2;
	new_data = ALLOC(char *, new_cap);
	if (!new_data)
		return (false);
	ft_memcpy(new_data, vec->data, vec->size * sizeof(char *));
	FREE(vec->data);
	vec->data = new_data;
	vec->capacity = new_cap;
	return (true);
}

/*
** Append a copy of string `s` to the vector.
** The vector stays null-terminated (data[size] == NULL).
** Returns true on success, false on allocation failure.
*/
bool	m_string_vector_append(t_string_vector *vec, const char *s)
{
	char	*dup;

	if (!vector_grow(vec, vec->size + 1))
		return (false);
	dup = ft_strdup(s);
	if (!dup)
		return (false);
	vec->data[vec->size] = dup;
	vec->size++;
	vec->data[vec->size] = NULL;
	return (true);
}

/*
** Return the string at `index`, or NULL if index is out of bounds.
*/
const char	*m_string_vector_at(t_string_vector *vec, size_t index)
{
	if (index >= vec->size)
		return (NULL);
	return (vec->data[index]);
}

/*
** Free the vector, all stored strings, and the vector struct itself.
*/
void	m_string_vector_free(t_string_vector *vec)
{
	size_t	i;

	if (!vec || !vec->data)
		return ;
	i = 0;
	while (i < vec->size)
	{
		FREE(vec->data[i]);
		i++;
	}
	FREE(vec->data);
	vec->data = NULL;
	vec->size = 0;
	vec->capacity = 0;
}
