/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 02:53:14 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/04 02:53:52 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static int	get_sign(const char *nptr, int *i)
{
	if (nptr[*i] == '-')
	{
		(*i)++;
		return (-1);
	}
	else if (nptr[*i] == '+')
	{
		(*i)++;
		return (1);
	}
	return (1);
}

static int	check_ovflow(int result, int digit, int sign)
{
	if (sign == -1 && result > (LONG_MAX - digit) / 10)
	{
		return ((int)LONG_MIN);
	}
	else if (sign == 1 && result > (LONG_MAX - digit) / 10)
	{
		return ((int)LONG_MAX);
	}
	return (0);
}

int	ft_atoi(const char *nptr)
{
	long	result;
	int		sign;
	int		i;
	int		digit;
	int		ovflow;

	i = 0;
	result = 0;
	while (nptr[i] == ' ' || nptr[i] == '\f' || nptr[i] == '\n'
		|| nptr[i] == '\r' || nptr[i] == '\t' || nptr[i] == '\v')
		i++;
	sign = get_sign(nptr, &i);
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		digit = nptr[i] - '0';
		ovflow = check_ovflow(result, digit, sign);
		if (ovflow != 0)
		{
			return (ovflow);
		}
		result = result * 10 + digit;
		i++;
	}
	return (sign * result);
}
