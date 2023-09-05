# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: max <max@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/05 21:27:42 by max               #+#    #+#              #
#    Updated: 2023/09/05 21:31:02 by max              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

##-----ALIAS-----##

NAME 	= 	webserv
CC 		= 	g++
FLAGS 	= 	-Wall -Wextra -Werror -std=c++98
RM		= 	rm -rf
MKDIR	= 	mkdir -p
AR		=	ar rcs


##-----VARIABLES-----##
COLOR			= \033
WHITE			= $(COLOR)[97m
CYAN			= $(COLOR)[96m
GREEN			= $(COLOR)[32m
BLUE			= $(COLOR)[34m
RED 			= $(COLOR)[31m
YELLOW			= $(COLOR)[33m
MAGENTA 		= $(COLOR)[35m
BOLD			= $(COLOR)[1m
RESET			= $(COLOR)[0m
RESET_BOLD		= $(COLOR)[21m

##-----PATHS-----##

PROJECT_DIR		= 	src/
HEADERS_DIR 	= 	includes/

OBJS_DIR 		= 	obj/

##-----FILES-----##

SRC_HEADER		=	Zombie.hpp
SRC_FILES		=	main.cpp \
					Zombie.cpp \
					newZombie.cpp \
					randomChump.cpp


SRCS			=	$(addprefix $(PROJECT_DIR),$(SRC_FILES))


##-----SOURCES-----##

OBJ				=	$(SRCS:.cpp=.o)
OBJS			=	$(addprefix $(OBJS_DIR),$(OBJ))
HEADERS			=	$(addprefix $(HEADERS_DIR),$(SRC_HEADER))

$(OBJS_DIR)$(PROJECT_DIR)%.o : $(PROJECT_DIR)%.cpp $(HEADERS)
	@mkdir -p $(OBJS_DIR)$(PROJECT_DIR)
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

#-----ASCII ART SISI LE S-----#
asciiart:
	@echo "$(RED)                                                                                  	"
	@echo "                                                                         	           	"
	@echo " ██████╗██████╗ ██████╗                          						            	"                        
	@echo "██╔════╝██╔══██╗██╔══██╗    						                                      	"                   
	@echo "██║     ██████╔╝██████╔╝                                             				 	"               
	@echo "██║     ██╔═══╝ ██╔═══╝                                          			         	"           
	@echo "╚██████╗██║     ██║                                              	      		       	"       
	@echo " ╚═════╝╚═╝     ╚═╝                                                               		"   
	@echo "$(BLUE)                                                                                  "
	@echo " ██████╗  ██████╗                                                                    	"
	@echo "██╔═████╗██╔═████╗                                                                   	"
	@echo "██║██╔██║██║██╔██║                                                                   	"
	@echo "████╔╝██║████╔╝██║                                                                   	"
	@echo "╚██████╔╝╚██████╔╝                                                                   	"
	@echo " ╚═════╝  ╚═════╝                                                                    	"
	@echo "$(GREEN)                                                                                 "
	@echo "██████╗ ██╗   ██╗    ███╗   ███╗███████╗████████╗ ██████╗  ██████╗██╗  ██╗██╗     ██╗	"
	@echo "██╔══██╗╚██╗ ██╔╝    ████╗ ████║██╔════╝╚══██╔══╝██╔═══██╗██╔════╝██║ ██╔╝██║     ██║	"
	@echo "██████╔╝ ╚████╔╝     ██╔████╔██║███████╗   ██║   ██║   ██║██║     █████╔╝ ██║     ██║	"
	@echo "██╔══██╗  ╚██╔╝      ██║╚██╔╝██║╚════██║   ██║   ██║   ██║██║     ██╔═██╗ ██║     ██║	"
	@echo "██████╔╝   ██║       ██║ ╚═╝ ██║███████║   ██║   ╚██████╔╝╚██████╗██║  ██╗███████╗██║	"
	@echo "╚═════╝    ╚═╝       ╚═╝     ╚═╝╚══════╝   ╚═╝    ╚═════╝  ╚═════╝╚═╝  ╚═╝╚══════╝╚═╝	"
	@echo "$(RESET)                                                                                 "