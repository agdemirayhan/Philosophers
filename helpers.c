/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:18:42 by aagdemir          #+#    #+#             */
/*   Updated: 2024/11/30 11:19:42 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	ft_usleep(size_t milliseconds)
{
	size_t	start;

	start = get_current_time();
	while ((get_current_time() - start) < milliseconds)
		usleep(50);
}

// void	cleanup(t_data *data)
// {
// 	int	i;

// 	i = 0;
// 	while (i < data->num_of_philos)
// 	{
// 		pthread_mutex_destroy(&data->forks[i].mutex_fork);
// 		i++;
// 	}
// 	pthread_mutex_destroy(&data->mutex_print);
// 	pthread_mutex_destroy(&data->mutex_start);
// 	pthread_mutex_destroy(&data->mutex_time);
// 	pthread_mutex_destroy(&data->mutex_last_time);
// 	pthread_mutex_destroy(&data->mutex_meal);
// 	pthread_mutex_destroy(&data->mutex_index);
// 	pthread_mutex_destroy(&data->mutex_thread);
// 	pthread_mutex_destroy(&data->mutex_isfinish);
// 	free(data->philos);
// }