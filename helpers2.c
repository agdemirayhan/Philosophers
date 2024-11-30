/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers2.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 11:20:16 by aagdemir          #+#    #+#             */
/*   Updated: 2024/11/30 11:21:41 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_helper(t_data *data, int type, int i, size_t time)
{
	if (!data->is_finish)
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
	else if (data->is_finish && type == 4)
	{
		printf("Philosophers ate %d times", data->count_meals);
		return ;
	}
}

void	print_handler(t_data *data, int type, int i)
{
	size_t	time;

	pthread_mutex_lock(&data->mutex_print);
	pthread_mutex_lock(&data->mutex_time);
	time = get_current_time() - data->start_time;
	pthread_mutex_unlock(&data->mutex_time);
	pthread_mutex_lock(&data->mutex_isfinish);
	pthread_mutex_unlock(&data->mutex_isfinish);
	time = get_current_time() - data->start_time;
	print_helper(data, type, i, time);
	pthread_mutex_unlock(&data->mutex_print);
}

void	init_helper(t_data *data)
{
	pthread_mutex_init(&(data->mutex_print), NULL);
	pthread_mutex_init(&(data->mutex_start), NULL);
	pthread_mutex_init(&(data->mutex_time), NULL);
	pthread_mutex_init(&(data->mutex_last_time), NULL);
	pthread_mutex_init(&(data->mutex_meal), NULL);
	pthread_mutex_init(&(data->mutex_index), NULL);
	pthread_mutex_init(&(data->mutex_thread), NULL);
	pthread_mutex_init(&(data->mutex_isfinish), NULL);
}

void	initialize(t_data *data, char **argv)
{
	int	i;

	i = -1;
	data->num_of_philos = ft_atoi(argv[1]);
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	data->index = 0;
	data->is_finish = 0;
	if (argv[5])
		data->count_meals = ft_atoi(argv[5]);
	while (++i < data->num_of_philos)
	{
		data->philos[i].id = i + 1;
		if (argv[5])
			data->philos[i].count_meal = ft_atoi(argv[5]);
		data->philos[i].last_time_eat = get_current_time();
		pthread_mutex_init(&data->philos[i].mutex_fork, NULL);
	}
}
