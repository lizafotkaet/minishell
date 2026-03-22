/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:16:31 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:31:47 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENV_H
# define ENV_H

# include <stddef.h>
# include <stdbool.h>
# include "result.h"
# include "quotes.h"

typedef struct s_env_key_value_pair
{
	char	*key;
	char	*value;
}	t_env_key_value_pair;

typedef struct s_result_t_env_key_value_pair
{
	bool					is_error;
	t_env_key_value_pair	value;
}	t_result_t_env_key_value_pair;

t_env_key_value_pair			m_env_key_value_pair_new(void);
void							m_env_key_value_pair_free(
									t_env_key_value_pair *pair);
bool							m_env_key_value_pair_key_equals(
									t_env_key_value_pair pair,
									const char *key);
t_result_t_env_key_value_pair	parse_env_var(const char *str);
t_result_t_char_ptr				m_env_key_value_pair_to_c_str(
									t_env_key_value_pair pair);

# define ENV_INITIAL_CAPACITY 32

typedef struct s_env
{
	t_env_key_value_pair	*pairs;
	int						count;
	int						capacity;
	int						previous_command_exit_code;
}	t_env;

typedef struct s_result_t_env
{
	bool	is_error;
	t_env	value;
}	t_result_t_env;

void							m_env_set_exit_code(t_env *env, int code);
const char						*m_env_find_value(t_env env, const char *key);
bool							m_env_append(
									t_env *env, t_env_key_value_pair pair);
bool							m_env_remove(t_env *env, const char *key);
t_result_t_env					parse_env(char **envp);
void							free_env(t_env *env);
char							**m_env_to_char_ptr(t_env env);
size_t							env_var_name_len(const char *s);
t_result_t_char_ptr				substitute_env(const char *input, t_env env);

#endif
