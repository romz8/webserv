# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rjobert <rjobert@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 18:19:03 by rjobert           #+#    #+#              #
#    Updated: 2024/04/03 19:59:20 by rjobert          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
FLAGS = -MMD # -Wall -Wextra -Werror -std=c++98
NAME = testserv
RM = rm -rf

SRC_DIR = ./srcs/
INC_DIR = ./include/
OBJS_PATH	= ./OBJS/

SRC_NAME = main.cpp networking/Socket.cpp server/Server.cpp http/Header.cpp http/Response.cpp
INC_NAME = Socket.hpp Server.hpp Location.hpp Header.hpp Response.hpp colors.h


SRC = $(addprefix $(SRC_DIR), $(SRC_NAME))
INCS = $(addprefix $(INC_DIR), $(INC_NAME))
ALL_INC = -I $(INC_DIR)

OBJS = $(SRC:$(SRC_DIR)%.cpp=$(OBJS_PATH)%.o)
DEPS	= $(addprefix $(OBJS_PATH), $(OBJ:.o=.d))

all: $(OBJS_PATH) $(NAME)

-include $(DEPS)
$(NAME): $(OBJS)
	$(CC) $(FLAGS) $(OBJS) $(ALL_INC) -o $(NAME)


$(OBJS_PATH):
	mkdir -p $(OBJS_PATH)
	# mkdir -p $(OBJS_PATH)/networking
	
$(OBJS_PATH)%.o: $(SRC_DIR)%.cpp Makefile
	@mkdir -p $(dir $@) 
	$(CC) $(FLAGS) $(ALL_INC) -c $< -o $@

clean:
	$(RM) $(OBJS) $(DEPS)
	$(RM) $(OBJS_PATH)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
	
