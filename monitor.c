/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 18:37:45 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:01:57 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	*monitor_thread(void *args)
{
	t_data *data;
	int i;
	size_t current_time;
	int count_meal;

	data = (t_data *)args;
	while (!data->is_finish)
	{
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
				return (NULL);
			}
			i++;
			pthread_mutex_unlock(&data->mutex_last_time);
		}
		i = 0;
		while (i < data->num_of_philos && data->fifth_arg)
		{
			pthread_mutex_lock(&data->mutex_meal);
			count_meal = data->philos[i].count_meal;
			pthread_mutex_unlock(&data->mutex_meal);
			if (count_meal && count_meal != 0)
			{
				break ;
			}
			else if (data->fifth_arg)
			{
				pthread_mutex_lock(&data->mutex_isfinish);
				data->is_finish = 1;
				pthread_mutex_unlock(&data->mutex_isfinish);
				print_handler(data, 4, i);
				return (NULL);
			}
			i++;
		}
	}
	return (NULL);
}