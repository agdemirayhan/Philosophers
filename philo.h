/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:22:38 by aagdemir          #+#    #+#             */
/*   Updated: 2024/11/30 11:22:41 by aagdemir         ###   ########.fr       */
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

typedef struct s_philo
{
	pthread_t		thread;
	int				id;
	int				count_meal;
	size_t			last_time_eat;
	pthread_mutex_t	mutex_fork;
	struct s_data	*data;
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
void				*monitor_thread(void *args);
size_t				get_current_time(void);
void				print_handler(t_data *data, int type, int i);
void				*philo_routine(void *args);
void				ft_usleep(size_t milliseconds);
void				cleanup(t_data *data);
void				initialize(t_data *data, char **argv);

#endif
