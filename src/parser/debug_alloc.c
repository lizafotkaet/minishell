/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   debug_alloc.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/19 13:02:03 by liza              #+#    #+#             */
/*   Updated: 2026/03/22 18:58:30 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "debug_alloc.h"

#ifdef DEBUG
int	g_alloc_counter = 0;
int	g_fail_on_alloc = 0;
int	g_debug_alloc_verbose = 0;
#endif
