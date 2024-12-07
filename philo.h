#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

# define WRONG_INPUT 1
# define MALLOC_ERROR 2

# define FORKS "has taken a fork"
# define THINK "is thinking"
# define SLEEP "is sleeping"
# define EAT "is eating"
# define DIED "died"

// Forward declaration of t_data
struct s_data;

typedef struct s_philo
{
	pthread_t		thread;
	int				id;
	int				count_meal;
	int				left_fork;
	int				right_fork;
	size_t			last_time_eat;
	pthread_mutex_t	lock;
	struct s_data *data; // Use the forward deasdasdclaration for the pointer
}					t_philo;

typedef struct	s_forks
{
	int				occ;
	pthread_mutex_t	*lock;
}					t_forks;

typedef struct s_data
{
	t_philo			*philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				is_finish;
	int				index;
	int 			fifth_arg;
	int				num_of_philos;
	int				finished_philos;
	int				count_meals;
	size_t			start_time;
	pthread_mutex_t	print;
	pthread_mutex_t	meal;
	pthread_mutex_t	*finish;
	pthread_mutex_t	full_mutex;
	t_forks			*forks;
}					t_data;

int					check_input(int argc, char **argv);
long				ft_atoi(const char *str);
// long				current_time_ms(void);
// void				*philosopher_thread(void *arg);
// int					initialize(t_params *params, int argc, char **argv);
// void				start_simulation(t_params *params);
// void				*monitor_thread(void *arg);
// void				cleanup(t_params *params);

#endif
