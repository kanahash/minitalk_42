/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/06 16:28:55 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/06 16:32:10 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static int	handle_format(const char c, va_list args)
{
	if (c == 'c')
		return (print_char(va_arg(args, int)));
	else if (c == 's')
		return (print_str(1, va_arg(args, char *)));
	else if (c == 'p')
		return (put_pointer(va_arg(args, void *)));
	else if (c == 'd' || c == 'i')
		return (handle_decimal(va_arg(args, int)));
	else if (c == 'u')
		return (handle_unsigned(va_arg(args, unsigned int)));
	else if (c == 'x')
		return (print_hex(va_arg(args, unsigned int), 0));
	else if (c == 'X')
		return (print_hex(va_arg(args, unsigned int), 1));
	else if (c == '%')
		return (write(1, "%", 1));
	return (0);
}

int	ft_printf(const char *str, ...)
{
	va_list	args;
	int		total_char;

	va_start(args, str);
	total_char = 0;
	while (*str)
	{
		if (*str == '%')
		{
			str++;
			total_char += handle_format(*str, args);
		}
		else
			total_char += write(1, str, 1);
		str++;
	}
	va_end(args);
	return (total_char);
}
