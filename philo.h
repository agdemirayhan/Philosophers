#ifndef PHILO_H
# define PHILO_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philo
{
	int				id;
	pthread_t		thread;
	int				eating;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	int				meals_eaten;
	size_t			last_meal;
	size_t			time_to_die;
	size_t			time_to_eat;
	size_t			time_to_sleep;
	size_t			start_time;
	int				num_of_philos;
	int				num_times_to_eat;
	pthread_mutex_t	*write_lock;
	pthread_mutex_t	*dead_lock;
	pthread_mutex_t	*meal_lock;
	struct s_params	*params;
}					t_philo;

typedef struct s_params
{
	int				dead_flag;
	pthread_mutex_t	dead_lock;
	pthread_mutex_t	meal_lock;
	pthread_mutex_t	write_lock;
	int				philos_done;
	t_philo			*philos;
}					t_params;

long				current_time_ms(void);
void				*philosopher_thread(void *arg);
int					initialize(t_params *params, int argc, char **argv);
void				start_simulation(t_params *params);
void				*monitor_thread(void *arg);
void				cleanup(t_params *params);

#endif
