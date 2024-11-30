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
	pthread_mutex_lock(&data->philos[i].mutex_fork);
	pthread_mutex_lock(&data->philos[next].mutex_fork);
	print_handler(data, 0, i);
	pthread_mutex_lock(&data->mutex_last_time);
	data->philos[i].last_time_eat = get_current_time();
	pthread_mutex_unlock(&data->mutex_last_time);
	ft_usleep(data->time_to_eat);
	pthread_mutex_lock(&data->mutex_meal);
	if (philo->count_meal > 0)
	{
		philo->count_meal--;
		if (philo->count_meal == 0)
			data->finished_philos++;
	}
	pthread_mutex_unlock(&data->mutex_meal);
	pthread_mutex_unlock(&data->philos[next].mutex_fork);
	pthread_mutex_unlock(&data->philos[i].mutex_fork);
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

	philo = (t_philo *)args;
	data = philo->data;
	i = philo->id - 1;
	pthread_mutex_lock(&data->mutex_index);
	next = (i + 1) % data->num_of_philos;
	pthread_mutex_unlock(&data->mutex_index);
	if (data->num_of_philos == 1)
		return (NULL);
	pthread_mutex_lock(&data->mutex_start);
	pthread_mutex_unlock(&data->mutex_start);
	if (data->num_of_philos != 1 && (i + 1) % 2 == 1)
		oddnumber_printer(data, i);
	pthread_mutex_lock(&data->mutex_last_time);
	philo->last_time_eat = get_current_time();
	pthread_mutex_unlock(&data->mutex_last_time);
	while (1)
		routine_loop(data, i, next, philo);
	return (NULL);
}
