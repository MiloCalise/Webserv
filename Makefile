NAME		= webserv
CPP			= c++
CPPFLAGS	= -Wall -Wextra -Werror -std=c++98 -Iincludes -g3

SRCS		= srcs/main.cpp \
			  srcs/Socket/Socket.cpp \
			  srcs/Config/Config.cpp \
			  srcs/Config/LocationConfig.cpp \
			  srcs/Config/ServerConfig.cpp \
			  srcs/Parsing/Parsing.cpp \
			  srcs/Parsing/ServerParsing.cpp \
			  srcs/Parsing/LocationParsing.cpp \
			  srcs/Server/Server.cpp

OBJS_DIR	= objs
OBJS		= $(SRCS:srcs/%.cpp=$(OBJS_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: srcs/%.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
