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
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	long			last_meal_time;
	int				meals_eaten;
	struct s_params	*params;
}					t_philo;

typedef struct s_params
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				meals_required;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	t_philo			*philosophers;
	long			start_time;
	int dead; // <-- Flag to indicate if any philosopher is dead
}					t_params;

long				current_time_ms(void);
void				*philosopher_thread(void *arg);
int					initialize(t_params *params, int argc, char **argv);
void				start_simulation(t_params *params);
void				*monitor_thread(void *arg);
void				cleanup(t_params *params);

#endif
