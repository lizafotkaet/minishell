/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ebarbash <ebarbash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:11:36 by sergei_pilm       #+#    #+#             */
/*   Updated: 2026/03/22 21:44:45 by ebarbash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include "libft.h"

static double	parse_fraction(char *temp, int *error)
{
	double	fraction;

	fraction = ft_atoi(temp, error);
	while (*temp && *temp >= '0' && *temp <= '9')
	{
		fraction /= 10;
		temp++;
	}
	return (fraction);
}

double	ft_atof(const char *str)
{
	int		whole;
	double	fraction;
	char	*temp;
	int		error;
	int		is_negative;

	error = 0;
	is_negative = 0;
	while (*str == ' ' || (*str >= '\t' && *str <= '\r'))
		str++;
	if (*str == '-')
		is_negative = 1;
	whole = ft_atoi(str, &error);
	temp = ft_strchr(str, '.');
	if (!temp)
		temp = ft_strchr(str, ',');
	if (!temp)
		return (whole);
	temp++;
	fraction = parse_fraction(temp, &error);
	if (is_negative)
		return (whole - fraction);
	else
		return (whole + fraction);
}
