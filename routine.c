/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 19:16:04 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:24:34 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	philo_check_finish(t_data *data)
{
	int	finish;

	pthread_mutex_lock(&data->mutex_isfinish);
	finish = data->is_finish;
	pthread_mutex_unlock(&data->mutex_isfinish);
	return (finish);
}

static void	philo_handle_single(t_data *data, int i)
{
	pthread_mutex_lock(&data->philos[i].mutex_fork);
	ft_usleep(data->time_to_die);
	pthread_mutex_unlock(&data->philos[i].mutex_fork);
}

void	*philo_routine(void *args)
{
	t_philo	*philo;
	t_data	*data;
	int		i;
	int		next;

	philo = (t_philo *)args;
	data = philo->data;
	i = philo->id - 1;
	next = (i + 1) % data->num_of_philos;
	pthread_mutex_lock(&data->mutex_start);
	pthread_mutex_unlock(&data->mutex_start);
	if (data->num_of_philos == 1)
		return (philo_handle_single(data, i), NULL);
	if (i % 2 == 1)
		ft_usleep(data->time_to_eat / 2);
	while (!philo_check_finish(data))
	{
		philo_take_forks(data, i, next);
		philo_eat(data, philo);
		philo_drop_forks(data, i, next);
		philo_sleep_and_think(data, i);
	}
	return (NULL);
}
