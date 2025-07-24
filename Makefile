# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/04 02:54:38 by kanahash          #+#    #+#              #
#    Updated: 2025/07/24 17:11:27 by kanahash         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minitalk

CFLAGS = -Wall -Wextra -Werror

SERVER = server
CLIENT = client
SERVER_BONUS = server_bonus
CLIENT_BONUS = client_bonus

RM = rm -f
CC = cc

SRC_CLIENT = src/client.c
SRC_SERVER = src/server.c
SRC_CLIENT_BONUS = bonus/client_bonus.c
SRC_SERVER_BONUS = bonus/server_bonus.c
SRC_LIB = libft/ft_atoi.c libft/ft_memset.c\
		ft_printf/ft_printf.c ft_printf/ft_putchar_fd.c ft_printf/handle_decimal.c ft_printf/handle_unsigned.c ft_printf/print_char.c ft_printf/print_hex.c ft_printf/print_str.c ft_printf/put_pointer.c 

OBJ = $(SRC_LIB:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o) $(OBJ)
OBJ_SERVER = $(SRC_SERVER:.c=.o) $(OBJ)
OBJ_CLIENT_BONUS = $(SRC_CLIENT_BONUS:.c=.o) $(OBJ)
OBJ_SERVER_BONUS = $(SRC_SERVER_BONUS:.c=.o) $(OBJ)


all: $(CLIENT) $(SERVER)

bonus:$(CLIENT_BONUS) $(SERVER_BONUS)

$(CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -I ./ $(OBJ_CLIENT) -o $(CLIENT)

$(SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -I ./ $(OBJ_SERVER) -o $(SERVER)

$(CLIENT_BONUS): $(OBJ_CLIENT_BONUS)
	$(CC) $(CFLAGS) -I ./ $(OBJ_CLIENT_BONUS) -o $(CLIENT_BONUS)

$(SERVER_BONUS): $(OBJ_SERVER_BONUS)
	$(CC) $(CFLAGS) -I ./ $(OBJ_SERVER_BONUS) -o $(SERVER_BONUS)

.c.o:
	$(CC) $(CFLAGS) -I ./ -c $< -o $@

clean:
	$(RM) $(OBJ_SERVER) $(OBJ_CLIENT) $(OBJ_SERVER_BONUS) $(OBJ_CLIENT_BONUS) Z$(OBJ)

fclean: clean
	$(RM) $(CLIENT) $(SERVER) $(CLIENT_BONUS) $(SERVER_BONUS)

re: fclean all

re_bonus: fclean bonus

.PHONY: all clean fclean re
