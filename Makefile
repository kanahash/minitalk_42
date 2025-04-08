# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: kanahash <kanahash@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/04/04 02:54:38 by kanahash          #+#    #+#              #
#    Updated: 2025/04/06 18:07:45 by kanahash         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = minitalk

CFLAGS = -Wall -Wextra -Werror
SERVER = server
CLIENT = client
RM = rm -f
CC = cc
SRC_CLIENT = src/client.c
SRC_SERVER = src/server.c
SRC_LIB = libft/ft_atoi.c libft/ft_memset.c\
		ft_printf/ft_printf.c ft_printf/ft_putchar_fd.c ft_printf/handle_decimal.c ft_printf/handle_unsigned.c ft_printf/print_char.c ft_printf/print_hex.c ft_printf/print_str.c ft_printf/put_pointer.c 

OBJ = $(SRC_LIB:.c=.o)
OBJ_CLIENT = $(SRC_CLIENT:.c=.o) $(OBJ)
OBJ_SERVER = $(SRC_SERVER:.c=.o) $(OBJ)


all: $(NAME)

$(NAME): $(CLIENT) $(SERVER)

$(CLIENT): $(OBJ_CLIENT)
	$(CC) $(CFLAGS) -I ./ $(OBJ_CLIENT) -o client

$(SERVER): $(OBJ_SERVER)
	$(CC) $(CFLAGS) -I ./ $(OBJ_SERVER) -o server

.c.o:
	$(CC) $(CFLAGS) -I ./ -c $< -o $@

clean:
	$(RM) $(OBJ_SERVER) $(OBJ_CLIENT)

fclean: clean
	$(RM) $(CLIENT) $(SERVER)

re: fclean all

.PHONY: all clean fclean re
