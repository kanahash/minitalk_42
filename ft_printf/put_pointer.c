/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   put_pointer.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 16:34:36 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/06 16:34:39 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

int	put_pointer(void *ptr)
{
	uintptr_t	address;
	int			count;

	count = 0;
	if (!ptr)
	{
		count += write(1, "(nil)", 5);
		return (count);
	}
	address = (uintptr_t)ptr;
	count += write(1, "0x", 2);
	count += print_hex(address, 0);
	return (count);
}
