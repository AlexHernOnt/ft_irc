# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ahernand <ahernand@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/04/08 16:49:07 by ahernand          #+#    #+#              #
#    Updated: 2023/04/08 17:44:12 by ahernand         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_irc

CXX = clang++ 

CPPFLAGS = -std=c++98 # -Wall -Wextra -Werrors

SRCS =	main.cpp #client.cpp

OBJS = main.o
		
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