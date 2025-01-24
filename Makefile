NAME = landscape_visualizer

CXXFLAGS = -Wall -Wextra -O3

CC = c++

SRC = main.cpp 

OBJ = $(SRC:.cpp=.o)

INC = -lSDL2 -lm

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(INC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INC)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
