NAME = philo

CC = cc 
CFLAGS = -Wall -Wextra -Werror 
SRC = philo.c check_input.c utils.c print.c initialize.c monitor.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ) -lpthread

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all
