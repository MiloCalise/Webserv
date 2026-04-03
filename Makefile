NAME		= webserv
CPP			= c++
CPPFLAGS	= -Wall -Wextra -Werror -std=c++98 -Iincludes -g3

SRCS		= srcs/main.cpp srcs/config.cpp

OBJS		= $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CPP) $(CPPFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp includes/config.hpp
	$(CPP) $(CPPFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
