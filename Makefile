# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jsebasti <jsebasti@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/27 18:19:03 by rjobert           #+#    #+#              #
#    Updated: 2024/05/23 13:04:12 by jsebasti         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
FLAGS = -MMD -std=c++98 #-Wall -Wextra -Werror -Wpedantic -fsanitize=address,undefined -g
NAME = testserv
RM = rm -rf
NPD = --no-print-directory

SRC_DIR = ./srcs/
INC_DIR = ./include/ ./include/Config/
OBJS_PATH	= ./OBJS/

SRC_NAME = main.cpp networking/Socket.cpp server/Server.cpp http/Request.cpp http/Response.cpp \
	server/Location.cpp http/DirectoryListing.cpp CGI/CGI.cpp config/LocationConfig.cpp \
	config/serverConfig.cpp config/Parser.cpp config/Utils.cpp
INC_NAME = Socket.hpp Server.hpp Location.hpp Requst.hpp Response.hpp colors.h \
	LocationConfig.hpp ServerConfig.hpp DirectoryListing.hpp CGI.hpp Config/Directives.hpp Config/Parser.hpp \
	Config/Utils.hpp


SRC = $(addprefix $(SRC_DIR), $(SRC_NAME))
INCS = $(addprefix $(INC_DIR), $(INC_NAME))
ALL_INC = $(addprefix -I, $(INC_DIR))

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
