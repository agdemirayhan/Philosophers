/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 18:22:48 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 18:37:43 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
	{
		pthread_mutex_lock(&data->philos[i].mutex_fork);
		ft_usleep(data->time_to_die);
		pthread_mutex_unlock(&data->philos[i].mutex_fork);
		return (NULL);
	}
	if (i % 2 == 1)
		ft_usleep(data->time_to_eat / 2);
	while (1)
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
		pthread_mutex_unlock(&data->philos[i].mutex_fork);
		pthread_mutex_unlock(&data->philos[next].mutex_fork);
		print_handler(data, 1, i);
		ft_usleep(data->time_to_sleep);
		print_handler(data, 2, i);
		pthread_mutex_lock(&data->mutex_isfinish);
		if (data->is_finish)
		{
			pthread_mutex_unlock(&data->mutex_isfinish);
			return (NULL);
		}
		pthread_mutex_unlock(&data->mutex_isfinish);
	}
	return (NULL);
}

void	start_simulation(t_data *data)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&data->mutex_start);
	while (i < data->num_of_philos)
	{
		data->philos[i].data = data;
		pthread_create(&data->philos[i].thread, NULL, philo_routine,
			(void *)&data->philos[i]);
		i++;
	}
	pthread_mutex_lock(&data->mutex_time);
	data->start_time = get_current_time();
	pthread_mutex_unlock(&data->mutex_time);
	pthread_mutex_unlock(&data->mutex_start);
	pthread_create(&data->thread, NULL, monitor_thread, (void *)data);
}

int	main(int argc, char **argv)
{
	t_data	*data;
	int		i;

	i = 0;
	if (check_input(argc, argv) != 0)
	{
		printf("WRONG INPUTS!\n");
		return (WRONG_INPUT);
	}
	data = (t_data *)malloc(1 * sizeof(t_data));
	data->philos = (t_philo *)malloc((ft_atoi(argv[1]) * sizeof(t_philo)));
	initialize(data, argv);
	if (argv[5])
		data->fifth_arg = 1;
	start_simulation(data);
	while (i < data->num_of_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
	pthread_join(data->thread, NULL);
	cleanup(data);
	return (0);
}
