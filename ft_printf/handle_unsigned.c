/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_unsigned.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 16:31:05 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/06 16:36:05 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static void	print_unsignednbr(unsigned int n, int *count)
{
	if (n >= 10)
	{
		print_decimalnbr(n / 10, count);
		print_unsignednbr(n / 10, count);
	}
	ft_putchar_fd(n % 10 + '0', 1);
	(*count)++;
}

int	handle_unsigned(unsigned int n)
{
	int	count;

	count = 0;
	print_unsignednbr(n, &count);
	return (count);
}
