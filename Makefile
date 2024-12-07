NAME = philo

CC = cc
# CFLAGS = -g
# CFLAGS = -Wall -Wextra -Werror -fsanitize=address
SRC = philo.c check_input.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lpthread

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
