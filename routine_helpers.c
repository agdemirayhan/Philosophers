/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 19:20:42 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:24:25 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	philo_take_forks(t_data *data, int i, int next)
{
	if (i < next)
	{
		pthread_mutex_lock(&data->philos[i].mutex_fork);
		pthread_mutex_lock(&data->philos[next].mutex_fork);
	}
	else
	{
		pthread_mutex_lock(&data->philos[next].mutex_fork);
		pthread_mutex_lock(&data->philos[i].mutex_fork);
	}
	print_handler(data, 0, i);
}

void	philo_eat(t_data *data, t_philo *philo)
{
	pthread_mutex_lock(&data->mutex_last_time);
	philo->last_time_eat = get_current_time();
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
}

void	philo_drop_forks(t_data *data, int i, int next)
{
	pthread_mutex_unlock(&data->philos[i].mutex_fork);
	pthread_mutex_unlock(&data->philos[next].mutex_fork);
}

void	philo_sleep_and_think(t_data *data, int i)
{
	print_handler(data, 1, i);
	ft_usleep(data->time_to_sleep);
	print_handler(data, 2, i);
}
