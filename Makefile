NAME		= webserv
CPP			= c++
CPPFLAGS	= -Wall -Wextra -Werror -std=c++98 -Iincludes -g3

SRCS		= srcs/main.cpp srcs/Config.cpp srcs/Socket.cpp

OBJS_DIR	= objs
OBJS		= $(SRCS:srcs/%.cpp=$(OBJS_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: srcs/%.cpp | $(OBJS_DIR)
	$(CPP) $(CPPFLAGS) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
