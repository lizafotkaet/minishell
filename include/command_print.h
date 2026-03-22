/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_print.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 10:02:31 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 20:21:39 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
	This header was only needed for testing the parser
	to print out the pipeline 
	in the xml format
*/

#ifndef COMMAND_PRINT_H
# define COMMAND_PRINT_H

# include "command.h"
# include "buffer.h"

bool	write_command_xml(t_buffer *buf, t_command *cmd);

#endif
