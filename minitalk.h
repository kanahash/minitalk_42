/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minitalk.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 02:54:20 by kanahash          #+#    #+#             */
/*   Updated: 2025/04/08 22:37:08 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINITALK_H
# define MINITALK_H

# include <limits.h>
# include <signal.h>
# include <stdarg.h>
# include <stddef.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/types.h>
# include <unistd.h>

# define _XOPEN_SOURCE 700
# define SLEEP 500

// libft
int		ft_atoi(const char *nptr);
void	*ft_memset(void *s, int c, size_t len);

// ft_printf
int		ft_printf(const char *format, ...);
int		print_char(char c);
int		put_pointer(void *ptr);
int		print_str(int fd, char *str);
void	print_decimalnbr(int n, int *count);
int		handle_decimal(int n);
int		handle_unsigned(unsigned int n);
int		print_hex(uintptr_t num, int ft_isupper);
void	ft_putchar_fd(char c, int fd);

#endif