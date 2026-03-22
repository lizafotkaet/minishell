/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_print.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:16:50 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:43:31 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	This header was only needed to test the parser
	and to print out the pipeline (first convert
	it to xml format)
*/

#ifndef PIPELINE_PRINT_H
# define PIPELINE_PRINT_H

# include "minishell.h"
# include "pipeline.h"
# include "command_print.h"
# include "quotes.h"

t_result_t_char_ptr	pipeline_to_xml(t_pipeline *pl);

#endif
