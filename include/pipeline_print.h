/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_print.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 04:16:50 by liza              #+#    #+#             */
/*   Updated: 2026/03/21 04:16:51 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPELINE_PRINT_H
# define PIPELINE_PRINT_H

# include "pipeline.h"
# include "command_print.h"

RESULT(t_char_ptr)	pipeline_to_xml(t_pipeline *pl);

#endif
