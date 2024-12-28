/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 18:37:45 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:09:21 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_philo_death(t_data *data)
{
	int		i;
	size_t	current_time;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_lock(&data->mutex_last_time);
		current_time = get_current_time();
		if ((current_time
				- data->philos[i].last_time_eat > (size_t)data->time_to_die)
			&& current_time > data->philos[i].last_time_eat)
		{
			print_handler(data, 3, i);
			pthread_mutex_lock(&data->mutex_isfinish);
			data->is_finish = 1;
			pthread_mutex_unlock(&data->mutex_isfinish);
			pthread_mutex_unlock(&data->mutex_last_time);
			return (1);
		}
		pthread_mutex_unlock(&data->mutex_last_time);
		i++;
	}
	return (0);
}

static int	check_all_meals(t_data *data)
{
	int	i;
	int	count_meal;

	i = 0;
	while (i < data->num_of_philos && data->fifth_arg)
	{
		pthread_mutex_lock(&data->mutex_meal);
		count_meal = data->philos[i].count_meal;
		pthread_mutex_unlock(&data->mutex_meal);
		if (count_meal && count_meal != 0)
			return (0);
		i++;
	}
	if (data->fifth_arg)
	{
		pthread_mutex_lock(&data->mutex_isfinish);
		data->is_finish = 1;
		pthread_mutex_unlock(&data->mutex_isfinish);
		print_handler(data, 4, i - 1);
		return (1);
	}
	return (0);
}

void	*monitor_thread(void *args)
{
	t_data	*data;

	data = (t_data *)args;
	while (!data->is_finish)
	{
		if (check_philo_death(data))
			return (NULL);
		if (check_all_meals(data))
			return (NULL);
	}
	return (NULL);
}
