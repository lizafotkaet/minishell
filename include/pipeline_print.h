/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_print.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:16:50 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 17:41:42 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPELINE_PRINT_H
# define PIPELINE_PRINT_H

# include "pipeline.h"
# include "command_print.h"

t_result_t_char_ptr	pipeline_to_xml(t_pipeline *pl);

#endif
