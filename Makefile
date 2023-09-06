# # **************************************************************************** #
# #                                                                              #
# #                                                         :::      ::::::::    #
# #    Makefile                                           :+:      :+:    :+:    #
# #                                                     +:+ +:+         +:+      #
# #    By: max <max@student.42.fr>                    +#+  +:+       +#+         #
# #                                                 +#+#+#+#+#+   +#+            #
# #    Created: 2023/09/05 21:27:42 by max               #+#    #+#              #
# #    Updated: 2023/09/05 22:17:13 by max              ###   ########.fr        #
# #                                                                              #
# # **************************************************************************** #

# ##-----ALIAS-----##

# NAME 	= 	webserv
# CC 		= 	g++
# FLAGS 	= 	-Wall -Wextra -Werror -std=c++98
# RM		= 	rm -rf
# MKDIR	= 	mkdir -p
# MKDIR   =   mkdir -p

# ##-----PATHS-----##

# PROJECT_DIR		= 	Networking/
# HEADERS_DIR 	= 	Includes/

# OBJS_DIR 		= 	obj/
# DIRS			=	$(OBJS_DIR) \
# 					$(OBJS_DIR)Networking/ \
# 					$(OBJS_DIR)Networking/Servers/ \
# 					$(OBJS_DIR)Networking/Sockets/

# ##-----FILES-----##

# SRC_HEADER      =   AServer.hpp \
# 					BindingSocket.hpp \
# 					ListeningSocket.hpp \
# 					ASocket.hpp \
# 					ConnectingSocket.hpp \
# 					TestServer.hpp

# SRC_FILES       =   main.cpp \
# 					Servers/AServer.cpp \
# 					Servers/TestServer.cpp \
# 					Sockets/ASocket.cpp \
# 					Sockets/BindingSocket.cpp \
# 					Sockets/ConnectingSocket.cpp \
# 					Sockets/ListeningSocket.cpp


# SRCS			=	$(addprefix $(PROJECT_DIR),$(SRC_FILES))


# ##-----SOURCES-----##

# OBJ = $(notdir $(SRCS:.cpp=.o))
# OBJS = $(addprefix $(OBJS_DIR),$(OBJ))
# HEADERS			=	$(addprefix $(HEADERS_DIR),$(SRC_HEADER))

# $(OBJS_DIR)%.o : $(PROJECT_DIR)%.cpp $(HEADERS)
# 	@$(MKDIR) $(OBJS_DIR)
# 	@$(CC) $(FLAGS) -I$(HEADERS_DIR) -o $@ -c $<

# ##-----RULES-----##

# directories:
# 	@$(MKDIR) $(DIRS)

# all:						$(NAME)

# $(NAME): $(OBJS)
# 	@$(CC) $(FLAGS) $(OBJS) -o $(NAME)

# clean:
# 		@$(RM)  $(OBJS)
# 		@$(RM) $(OBJS_DIR)

# fclean: clean
# 		@$(RM) $(NAME)

# re:		fclean all

# .PHONY:	all fclean clean re



##-----ALIAS-----##

NAME 	= 	webserv
CC 		= 	g++
FLAGS 	= 	-Wall -Wextra -Werror -std=c++98
RM		= 	rm -rf
MKDIR	= 	mkdir -p
AR		=	ar rcs



##-----PATHS-----##

PROJECT_DIR		= 	Networking/
HEADERS_DIR 	= 	Includes/

DIRS = $(OBJS_DIR) \
       $(OBJS_DIR)Networking/ \
       $(OBJS_DIR)Networking/Servers/ \
       $(OBJS_DIR)Networking/Sockets/

OBJS_DIR 		= 	obj/

##-----FILES-----##


SRC_HEADER      =   AServer.hpp \
					BindingSocket.hpp \
					ListeningSocket.hpp \
					ASocket.hpp \
					ConnectingSocket.hpp \
					TestServer.hpp

SRC_FILES       =   main.cpp \
					Servers/AServer.cpp \
					Servers/TestServer.cpp \
					Sockets/ASocket.cpp \
					Sockets/BindingSocket.cpp \
					Sockets/ConnectingSocket.cpp \
					Sockets/ListeningSocket.cpp

SRCS			=	$(addprefix $(PROJECT_DIR),$(SRC_FILES))


##-----SOURCES-----##

OBJ				=	$(SRCS:.cpp=.o)
OBJS			=	$(addprefix $(OBJS_DIR),$(OBJ))
HEADERS			=	$(addprefix $(HEADERS_DIR),$(SRC_HEADER))

$(OBJS_DIR)$(PROJECT_DIR)%.o : $(PROJECT_DIR)%.cpp $(HEADERS)
	@mkdir -p $(OBJS_DIR)$(PROJECT_DIR)
	@mkdir -p $(DIRS)
	@$(CC) $(FLAGS) -o $(@) -c $(<)

##-----RULES-----##

all:						$(NAME)

$(NAME):	$(OBJS)
	@$(CC) $(FLAGS) $(^) -o $(@)

clean:
		@$(RM)  $(OBJS)
		@$(RM) $(OBJS_DIR)

fclean: clean
		@$(RM) $(NAME)

re:		fclean all

.PHONY:	all fclean clean re

