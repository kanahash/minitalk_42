/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_hex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 16:32:52 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/06 16:32:55 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

int	print_hex(uintptr_t num, int uppercase)
{
	char	digit;
	int		count;

	count = 0;
	if (num >= 16)
		count += print_hex(num / 16, uppercase);
	if ((num % 16) < 10)
		digit = '0' + (num % 16);
	else
	{
		if (uppercase)
			digit = 'A' + (num % 16 - 10);
		else
			digit = 'a' + (num % 16 - 10);
	}
	count += write(1, &digit, 1);
	return (count);
}
