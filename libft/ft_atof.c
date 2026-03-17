/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atof.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergei_pilman <sergei_pilman@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 18:11:36 by sergei_pilm       #+#    #+#             */
/*   Updated: 2025/08/18 20:24:15 by sergei_pilm      ###   ########.fr       */
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

// test:
// int main(void)
// {
//     char *test_cases[] = {
//         "123.45",
//         "-123.45", 
//         "0.5",
//         "-0.5",
//         "42",
//         "-42",
//         "0.0",
//         "-0.0",
//         "3.14159",
//         "-3.14159",
//         NULL
//     };
    
//     printf("Testing ft_atof vs standard atof:\n");
//     printf("%-12s %-15s %-15s %-10s\n", "Input", "ft_atof", "atof", "Match?");
//     printf("%-12s %-15s %-15s %-10s\n", "-----", "-------", "----", "------");
    
//     for (int i = 0; test_cases[i] != NULL; i++)
//     {
//         double my_result = ft_atof(test_cases[i]);
//         double std_result = atof(test_cases[i]);
        
//         printf("%-12s %-15.6f %-15.6f %-10s\n", 
//                test_cases[i], 
//                my_result, 
//                std_result,
//                (my_result == std_result) ? "YES" : "NO");
//     }
    
//     return 0;
// }