/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_convert_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asrichar <asrichar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 19:23:03 by asrichar          #+#    #+#             */
/*   Updated: 2026/03/22 19:25:02 by asrichar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/minishell.h"

int	count_redirs(t_command *cmd)
{
	int	n;
	int	j;

	n = 0;
	j = 0;
	while (j < cmd->redirect_count)
	{
		if (cmd->redirects[j].type != E_HEREDOC)
			n++;
		j++;
	}
	return (n);
}
