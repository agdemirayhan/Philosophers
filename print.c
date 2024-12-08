/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 18:27:40 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:03:50 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_types(t_data *data, int type, int i, size_t time)
{
	if (type == 0)
	{
		printf("%zu %d %s\n", time, data->philos[i].id, FORKS);
		printf("%zu %d %s\n", time, data->philos[i].id, FORKS);
		printf("%zu %d %s\n", time, data->philos[i].id, EAT);
	}
	else if (type == 1)
		printf("%zu %d %s\n", time, data->philos[i].id, SLEEP);
	else if (type == 2)
		printf("%zu %d %s\n", time, data->philos[i].id, THINK);
	else if (type == 3)
		printf("%zu %d %s\n", time, data->philos[i].id, DIED);
}

void	print_handler(t_data *data, int type, int i)
{
	size_t	time;

	pthread_mutex_lock(&data->mutex_print);
	pthread_mutex_lock(&data->mutex_time);
	time = get_current_time() - data->start_time;
	pthread_mutex_unlock(&data->mutex_time);
	pthread_mutex_lock(&data->mutex_isfinish);
	if (data->is_finish)
	{
		pthread_mutex_unlock(&data->mutex_isfinish);
		if (type == 4)
			printf("Philosophers ate %d times\n", data->count_meals);
		pthread_mutex_unlock(&data->mutex_print);
		return ;
	}
	pthread_mutex_unlock(&data->mutex_isfinish);
	print_types(data, type, i, time);
	pthread_mutex_unlock(&data->mutex_print);
}
