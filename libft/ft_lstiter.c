/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstiter.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 14:29:34 by ebarbash          #+#    #+#             */
/*   Updated: 2025/02/24 14:01:03 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

void	ft_lstiter(t_list *lst, void (*f)(int))
{
	while (lst)
	{
		f(lst->content);
		lst = lst->next;
	}
}
