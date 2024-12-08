/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 19:24:55 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:25:54 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
struct	s_data;

typedef struct s_philo
{
	pthread_t		thread;
	int				id;
	int				count_meal;
	size_t			last_time_eat;
	pthread_mutex_t	mutex_fork;
	struct s_data	*data; // Use the forward deasdasdclaration for the pointer
}					t_philo;

typedef struct s_data
{
	t_philo			*philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				is_finish;
	int				index;
	int				fifth_arg;
	int				num_of_philos;
	int				finished_philos;
	int				count_meals;
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
long				ft_atoi(const char *str);
size_t				get_current_time(void);
size_t				get_current_time(void);
void				cleanup(t_data *data);
void				ft_usleep(size_t milliseconds);
void				print_handler(t_data *data, int type, int i);
void				initialize(t_data *data, char **argv);
void				*monitor_thread(void *args);
void				*philo_routine(void *args);
// routine_helpers
void				philo_sleep_and_think(t_data *data, int i);
void				philo_drop_forks(t_data *data, int i, int next);
void				philo_eat(t_data *data, t_philo *philo);
void				philo_take_forks(t_data *data, int i, int next);

#endif
