/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:17:16 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 04:17:17 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPELINE_H
# define PIPELINE_H

# include "command.h"
# include "env.h"

typedef struct s_pipeline
{
	t_command	*commands;
	int			command_count;
}	t_pipeline;

DECLARE_RESULT(t_pipeline);

void					free_pipeline(t_pipeline *pipeline);
bool					read_heredocs(t_pipeline *pl);
bool					substitute_all_envs(t_token_vector *tokens,
							t_env env);
RESULT(t_pipeline)		create_pipeline(const char *input, t_env env);

#endif
