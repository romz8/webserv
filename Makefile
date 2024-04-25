# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/07/17 07:41:15 by jsebasti          #+#    #+#              #
#    Updated: 2024/04/26 00:58:47 by jsebasti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= webserv
NPD			= --no-print-directory
OBJ_DIR		= obj/


# ----Libraryes----
INC_DIR = inc/

INC = -I $(INC_DIR)defines -I $(INC_DIR)directives -I $(INC_DIR)parser -I $(INC_DIR)utils

# =============

# -------------
RM = rm -rf
MP = mkdir -p
CC = c++
CFLAGS = -Werror -Wextra -Wall -O3 -g -std=c++98 #-fsanitize=address
# =============

FILES = main Parser ParseContent ParseDirectives Signals Utils Server Directives Location

SRC = $(addsuffix .cpp, $(FILES))

vpath %.cpp src
vpath %.cpp src/directives
vpath %.cpp src/parser
vpath %.cpp src/utils

# -------------
OBJ = $(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))
DEP = $(addsuffix .d, $(basename $(OBJ)))
# =============

all:
	@$(MAKE) $(NAME) $(NPD)

$(OBJ_DIR)%.o: %.cpp
	@$(MP) $(dir $@)
	@$(CC) $(CFLAGS) -MMD $(INC) -c $< -o $@
	@echo "Object of the file $(basename $<) has been created 🤐"


$(NAME):: $(OBJ)
	@$(CC) $(CFLAGS) $(OBJ) -o $(NAME)
	@echo "Compiling $(NAME) 😈"

$(NAME)::
	@echo "Hello, $(NAME) already compiled 😇"

clean:
	@$(RM) $(OBJ_DIR)
	@echo "$(OBJ_DIR) and company leave 42 🗑"

fclean: clean
	@$(RM) $(NAME)
	@echo "$(NAME) has been black holed 🕳"

re:
	@$(MAKE) fclean $(NPD)
	@$(MAKE) $(NPD)
	
-include $(DEP)

.PHONY: all clean fclean re
