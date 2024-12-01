NAME = philo

CC = cc
CFLAGS = -g -fsanitize=address
# CFLAGS = -Wall -Wextra -Werror
SRC = philo.c check_input.c monitor.c routine.c helpers.c helpers2.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lpthread

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
