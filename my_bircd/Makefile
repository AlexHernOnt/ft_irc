# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ahernand <marvin@42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/03/04 16:17:43 by ahernand          #+#    #+#              #
#    Updated: 2023/03/04 20:12:16 by ahernand         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = bircd

all : ${NAME}


CC = GCC

CFLAGS = -I. -g3 -Wall -Werror


SRCS =	main.c		\
		init_env.c

OBJS = ${SRCS:.c=.o}

${NAME} :	${OBJS}
		${CC} -o ${NAME} ${OBJS} ${LDFLAGS}




# # # # # # # # # # # # 
#   C L E A N I N G   # 
# # # # # # # # # # # #

RM: rm -f

clean:
	${RM} ${OBJS} *~ #*#

fclean: clean
	${RM} ${NAME}

re: fclean all
