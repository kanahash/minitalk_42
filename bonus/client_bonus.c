/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_bonus.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 16:45:12 by kanahash          #+#    #+#             */
/*   Updated: 2025/07/24 16:45:15 by kanahash         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minitalk.h"

static volatile sig_atomic_t	*get_ack_ptr(void)
{
	static volatile sig_atomic_t	ack = 0;

	return (&ack);
}

static void	act_handler(int sig, siginfo_t *info, void *ucontext)
{
	volatile sig_atomic_t	*ack_ptr;

	(void)sig;
	(void)info;
	(void)ucontext;
	ack_ptr = get_ack_ptr();
	*ack_ptr = 1;
}

static void	send_char(pid_t server_pid, char c)
{
	int						i;
	int						ret;
	volatile sig_atomic_t	*ack_ptr;

	ack_ptr = get_ack_ptr();
	i = 7;
	while (i >= 0)
	{
		if ((c >> i) & 1)
			ret = kill(server_pid, SIGUSR1);
		else
			ret = kill(server_pid, SIGUSR2);
		if (ret == -1)
		{
			write(1, "kill failed\n", 12);
			exit(EXIT_FAILURE);
		}
		i--;
		usleep(SLEEP);
	}
	while (!(*ack_ptr))
		pause();
	*ack_ptr = 0;
}

static void	send_msg(pid_t server_pid, char *msg)
{
	size_t	len;

	len = 0;
	while (msg[len])
	{
		send_char(server_pid, msg[len]);
		len++;
	}
	send_char(server_pid, '\0');
}

int	main(int ac, char **av)
{
	pid_t				pid;
	struct sigaction	sa;

	if (ac != 3)
	{
		write(2, "Error: Invalid argument count\n", 30);
		write(2, "Usage: %s <server_pid> <message>\n", 34);
		return (1);
	}
	pid = ft_atoi(av[1]);
	if (pid <= 0)
	{
		write(2, "Error: Invalid PID\n", 20);
		return (1);
	}
	ft_memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = act_handler;
	sigemptyset(&sa.sa_mask);
	if ((sigaction(SIGUSR1, &sa, NULL) == -1) || (sigaction(SIGUSR2, &sa,
				NULL) == -1))
		return (1);
	if (*av[2])
		send_msg(pid, av[2]);
	return (0);
}
