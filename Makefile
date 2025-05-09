# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: shinckel <shinckel@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/11/26 21:27:04 by shinckel          #+#    #+#              #
#    Updated: 2025/05/08 18:17:43 by shinckel         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

SRCS =	src/main.cpp \
				src/core/Socket.cpp \
				src/core/Server.cpp \
        src/core/Client.cpp \
        src/core/Channel.cpp \
				src/adm/Manager.cpp \
				src/adm/ManagerUtils.cpp \
				src/actions/Invite.cpp \
				src/actions/Join.cpp \
				src/actions/Kick.cpp \
				src/actions/Mode.cpp \
				src/actions/ActionsUtils.cpp \
				src/actions/Privmsg.cpp \
				src/actions/Quit.cpp \
				src/actions/Topic.cpp \
				src/actions/Who.cpp \
        src/utils/Parser.cpp \

CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -Iinclude

OBJDIR = objs

OBJS = $(SRCS:src/%.cpp=$(OBJDIR)/%.o)

RED = \033[1;41m
GREEN = \033[1;42m
RESET = \033[0m

all: $(NAME)

$(OBJDIR)/%.o: src/%.cpp
			@mkdir -p $(dir $@)  # Create the OBJDIR directory and subdirectories if they don't exist
			@echo "Compiling $<..."
			$(CXX) $(CXXFLAGS) -c $< -o $@

$(NAME): $(OBJS)
			@echo "Linking $(NAME)..."
			$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
			@echo "$(GREEN) Ready to CHAT!!! $(RESET)"

clean:
		@rm -f $(OBJS)
		@echo "$(RED) Remove object files $(RESET)"

fclean: clean
		@rm -f $(NAME)
		@rm -rf $(OBJDIR)
		@echo "$(RED) Remove $(NAME) $(RESET)"

re: fclean all

.PHONY: all clean fclean re
