# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/08 16:49:07 by ahernand          #+#    #+#              #
#    Updated: 2023/04/29 15:51:40 by ahernand         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_irc

CXX = clang++ 

CPPFLAGS = -std=c++98 -Wall -Wextra -Werror

SRCS =	srcs/main.cpp srcs/server.cpp srcs/commands/join.cpp #client.cpp

OBJS = srcs/main.o srcs/server.o srcs/commands/join.o
		
all : $(NAME)

$(OBJS) : $(SRCS)

$(NAME) : $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@







# - - - - - - - - - - - - - - - - - #
# - - - - C L E A N I N G - - - - - #
# - - - - - - - - - - - - - - - - - #


clean:
	@rm -f $(OBJS)

fclean:
	@rm -f $(OBJS)
	@rm -f $(NAME)
	@rm -f ft_irc


re:	fclean all