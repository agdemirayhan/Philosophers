#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

# define WRONG_INPUT 1
# define MALLOC_ERROR 2

# define TAKE_FORKS "has taken a fork"
# define THINK "is thinking"
# define SLEEP "is sleeping"
# define EAT "is eating"
# define DIED "died"

typedef struct s_philo
{
	pthread_t		thread;
	int				id;
	int				count_meal;
	size_t			last_time_eat;
	pthread_mutex_t	mutex_fork;
}					t_philo;

typedef struct s_data
{
	t_philo			*philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				is_finish;
	int				index;
	int				num_of_philos;
	size_t			start_time;
	pthread_mutex_t	mutex_print;
	pthread_mutex_t	mutex_start;
	pthread_mutex_t	mutex_time;
	pthread_mutex_t	mutex_last_time;
	pthread_mutex_t	mutex_meal;
	pthread_mutex_t	mutex_index;
	pthread_mutex_t	mutex_thread;
	pthread_mutex_t	mutex_isfinish;
	pthread_t		thread;
}					t_data;

int					check_input(int argc, char **argv);
long	ft_atoi(const char *str);
// long				current_time_ms(void);
// void				*philosopher_thread(void *arg);
// int					initialize(t_params *params, int argc, char **argv);
// void				start_simulation(t_params *params);
// void				*monitor_thread(void *arg);
// void				cleanup(t_params *params);

#endif
