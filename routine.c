/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:16:04 by aagdemir          #+#    #+#             */
/*   Updated: 2024/11/30 11:17:31 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	routine_loop(t_data *data, int i, int next, t_philo *philo)
{
	int			left_fork;
	int			right_fork;
left_fork = philo->id - 1;
	right_fork = philo->id % data->num_of_philos;
	pthread_mutex_lock(data->forks[left_fork].lock_fork);
	pthread_mutex_lock(data->forks[right_fork].lock_fork);
	print_handler(data, 0, i);
	pthread_mutex_lock(&philo->philo);
		philo->last_time_eat = get_current_time();
		print_handler(data, 4,i);
		philo->count_meal++;
		pthread_mutex_unlock(&philo->philo);
	ft_usleep(data->time_to_eat);
	pthread_mutex_lock(&data->mutex_meal);
	if (philo->count_meal > data->meal_limit)
	{
			data->finished_philos++;
	}
	pthread_mutex_unlock(&data->mutex_meal);
	pthread_mutex_unlock(data->forks[left_fork].lock_fork);
	pthread_mutex_unlock(data->forks[right_fork].lock_fork);
	print_handler(data, 1, i);
	ft_usleep(data->time_to_sleep);
	print_handler(data, 2, i);
	if (data->is_finish)
	{
		pthread_mutex_unlock(&data->mutex_isfinish);
		return ;
	}
}

void	oddnumber_printer(t_data *data, int i)
{
	print_handler(data, 2, i);
	ft_usleep(data->time_to_eat);
}

void	*philo_routine(void *args)
{
	t_data	*data;
	int		i;
	int		next;
	t_philo	*philo;
	int			left_fork;
	int			right_fork;


	philo = (t_philo *)args;
	data = philo->data;
	left_fork = philo->id - 1;
	right_fork = philo->id % data->num_of_philos;
	i = philo->id - 1;
	pthread_mutex_lock(&philo->philo);
	philo->last_time_eat = get_current_time();
	pthread_mutex_unlock(&philo->philo);
	print_handler(data,2,i);
	if (data->num_of_philos == 1)
		return (NULL);
	if (data->num_of_philos != 1 && (i + 1) % 2 == 1)
		oddnumber_printer(data, i);
	while (!data->is_finish)
	{
		if(left_fork < right_fork)
		{
			pthread_mutex_lock(data->forks[left_fork].lock_fork);
			print_handler(data,0,philo->id);
			pthread_mutex_unlock(data->forks[left_fork].lock_fork);
		}
		routine_loop(data, i, next, philo);
	}
	return (NULL);
}
