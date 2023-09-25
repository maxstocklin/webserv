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

##-----ALIAS-----##

NAME 	= 	webserv
CC 		= 	g++
FLAGS 	= 	-Wall -Wextra -Werror -std=c++98
RM		= 	rm -rf
MKDIR	= 	mkdir -p
AR		=	ar rcs

ifeq ($(shell uname -s),Darwin)
OS = mac
else
OS = linux
endif

##-----PATHS-----##

PROJECT_DIR		= 	Networking/
HEADERS_DIR 	= 	Includes/

DIRS = $(OBJS_DIR) \
       $(OBJS_DIR)Networking/ \
       $(OBJS_DIR)Networking/Config/ \
       $(OBJS_DIR)Networking/Servers/ \
       $(OBJS_DIR)Networking/Errors/ \
       $(OBJS_DIR)Networking/Cgi/ \
       $(OBJS_DIR)Networking/Request/ \
       $(OBJS_DIR)Networking/Response/ \
       $(OBJS_DIR)Networking/Sockets/

OBJS_DIR 		= 	obj/

##-----FILES-----##


SRC_HEADER      =   AServer.hpp \
					BindingSocket.hpp \
					ASocket.hpp \
					ServerConfig.hpp \
					WebServer.hpp \
					AutoIndexGenerator.hpp \
					CgiHandler.hpp \
					Colors.hpp \
					MasterSocket.hpp \
					Response.hpp \
					ResponseHeader.hpp

SRC_FILES       =   main.cpp \
					Config/ServerConfig.cpp \
					Request/Request.cpp \
					Request/RequestMembers.cpp \
					Response/AutoIndexGenerator.cpp \
					Response/CgiHandler.cpp \
					Response/Response.cpp \
					Response/ResponseHeader.cpp \
					Servers/AServer.cpp \
					Servers/WebServer.cpp \
					Sockets/MasterSocket.cpp \
					Sockets/ASocket.cpp \
					Sockets/BindingSocket.cpp

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


##################################################
# TESTING
##################################################

test: test_$(OS)

test_setup: all
	@rm -rf test_us/root
	@mkdir -p test_us/root
	@cp test_us/index/* test_us/root/
	@cp test_us/root/index.html test_us/root/index_permission.html
	@chmod 000 test_us/root/index_permission.html
	@clang++ -o client test_us/client.cpp

test_mac: test_setup
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && ./client"'
	@osascript -e 'tell application "Terminal" to activate'
	./webserv test_us/conf/webserv.conf

test_linux: test_setup
	@x-terminal-emulator --working-directory=$$(pwd) -x "./client"
	./webserv test_us/conf/webserv.conf

bocal: bocal_$(OS)

bocal_mac: all
	@mkdir -p YoupiBanane/put_here
	@osascript -e 'tell application "Terminal" to do script "cd $(PWD) && clear && time ./test_mac/macos_tester http://localhost:8000"'
	@osascript -e 'tell application "Terminal" to activate'
	./webserv test_mac/mac.conf

bocal_linux: all
	@mkdir -p YoupiBanane/put_here
	@x-terminal-emulator --working-directory=$$(pwd) -x "time ./test_linux/ubuntu_tester http://localhost:8000"
	./webserv test_linux/linux.conf

.PHONY: libft clean fclean re test test_setup test_mac test_linux bocal bocal_mac bocal_linux

# .PHONY:	all fclean clean re

