/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_decimal.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 16:29:36 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/06 16:32:00 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

void	print_decimalnbr(int n, int *count)
{
	char	digit;

	if (n == INT_MIN)
	{
		write(1, "-2147483648", 11);
		*count += 11;
		return ;
	}
	if (n < 0)
	{
		ft_putchar_fd('-', 1);
		(*count)++;
		n = -n;
	}
	if (n >= 10)
		print_decimalnbr(n / 10, count);
	digit = '0' + (n % 10);
	write(1, &digit, 1);
	(*count)++;
}

int	handle_decimal(int n)
{
	int	count;

	count = 0;
	print_decimalnbr(n, &count);
	return (count);
}
