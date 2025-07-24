/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 16:45:36 by kanahash          #+#    #+#             */
/*   Updated: 2025/07/24 16:45:39 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static void	build_bit(int signal, unsigned char *current_char, int *bit_len)
{
	if (signal == SIGUSR1)
		*current_char |= (1 << (7 - *bit_len));
	(*bit_len)++;
}

static void	process_byte(unsigned char c)
{
	static char	buf[4096];
	static int	buf_idx = 0;

	if (c == '\0')
	{
		buf[buf_idx] = '\0';
		ft_printf("%s\n", buf);
		buf_idx = 0;
	}
	else
	{
		buf[buf_idx] = c;
		buf_idx++;
		if (buf_idx >= (int)(sizeof(buf) - 1))
		{
			buf[buf_idx] = '\0';
			ft_printf("%s", buf);
			buf_idx = 0;
		}
	}
}

static void	handle_signal(int signal, siginfo_t *info, void *ucontext)
{
	static unsigned char	current_char = 0;
	static int				bit_len = 0;
	pid_t					client_pid;
	union sigval			act_val;

	(void)ucontext;
	client_pid = info->si_pid;
	build_bit(signal, &current_char, &bit_len);
	if (bit_len == 8)
	{
		process_byte(current_char);
		act_val.sival_int = 0;
		if (sigqueue(client_pid, SIGUSR1, act_val) == -1)
			write(2, "Error: sigqueue failed\n", 23);
		current_char = 0;
		bit_len = 0;
	}
}

int	main(void)
{
	struct sigaction	sa;

	ft_printf("Server PID: %d\n", getpid());
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handle_signal;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGUSR1, &sa, NULL) == -1)
	{
		write(2, "Error: Failed to set SIGUSR1 handler\n", 37);
		return (1);
	}
	else if (sigaction(SIGUSR2, &sa, NULL) == -1)
	{
		write(2, "Error: Failed to set SIGUSR2 handler\n", 37);
		return (1);
	}
	while (1)
		pause();
	return (0);
}
