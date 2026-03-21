/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_print.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: liza <liza@student.42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 10:02:31 by liza              #+#    #+#             */
/*   Updated: 2026/03/20 10:02:32 by liza             ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_PRINT_H
# define COMMAND_PRINT_H

# include "command.h"
# include "buffer.h"

bool	write_command_xml(t_buffer *buf, t_command *cmd);

#endif
