/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:06:38 by aagdemir          #+#    #+#             */
/*   Updated: 2024/11/30 11:15:29 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int	meal_monitor(t_data *data)
{
	int		i;
	size_t	current_time;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_lock(&data->mutex_last_time);
		current_time = get_current_time();
		pthread_mutex_unlock(&data->mutex_last_time);
		if ((current_time
				- data->philos[i].last_time_eat > (size_t)data->time_to_die)
			&& current_time > data->philos[i].last_time_eat)
		{
			print_handler(data, 3, i);
			pthread_mutex_lock(&data->mutex_isfinish);
			data->is_finish = 1;
			pthread_mutex_unlock(&data->mutex_isfinish);
			return (-1);
		}
		i++;
	}
	return (0);
}

int	fiftharg_monitor(t_data *data)
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
			break ;
		else if (data->fifth_arg)
		{
			pthread_mutex_lock(&data->mutex_isfinish);
			data->is_finish = 1;
			pthread_mutex_unlock(&data->mutex_isfinish);
			print_handler(data, 4, i);
			return (-1);
		}
		i++;
	}
	return (0);
}

void	*monitor_thread(void *args)
{
	t_data	*data;
	int		i;
	size_t	current_time;

	data = (t_data *)args;
	while (!data->is_finish)
	{
		if (meal_monitor(data) == -1)
			return (NULL);
		if (fiftharg_monitor(data) == -1)
			return (NULL);
	}
	while (i < data->num_of_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
	return (NULL);
}
