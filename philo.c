/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 09:08:13 by aagdemir          #+#    #+#             */
/*   Updated: 2024/11/30 09:27:09 by aagdemir         ###   ########.fr       */
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

void	cleanup(t_data *data)
{
	int	i;

	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_mutex_destroy(&data->philos[i].mutex_fork);
		i++;
	}
	pthread_mutex_destroy(&data->mutex_print);
	pthread_mutex_destroy(&data->mutex_start);
	pthread_mutex_destroy(&data->mutex_time);
	pthread_mutex_destroy(&data->mutex_last_time);
	pthread_mutex_destroy(&data->mutex_meal);
	pthread_mutex_destroy(&data->mutex_index);
	pthread_mutex_destroy(&data->mutex_thread);
	pthread_mutex_destroy(&data->mutex_isfinish);
	free(data->philos);
}

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

void	routine_loop(t_data *data, int i, int next)
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
		return (pthread_mutex_unlock(&data->mutex_isfinish), NULL);
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
	{
		print_handler(data, 2, i);
		ft_usleep(data->time_to_eat);
	}
	pthread_mutex_lock(&data->mutex_last_time);
	philo->last_time_eat = get_current_time();
	pthread_mutex_unlock(&data->mutex_last_time);
	while (1)
		routine_loop();
	return (NULL);
}

void	*monitor_thread(void *args)
{
	t_data	*data;
	int		i;
	size_t	current_time;
	int		count_meal;

	data = (t_data *)args;
	while (!data->is_finish)
	{
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
				printf("check death:%lu\n", current_time
					- data->philos[i].last_time_eat);
				printf("current_time:%lu\n", current_time);
				printf("lasttime eat:%zu\n", data->philos[i].last_time_eat);
				print_handler(data, 3, i);
				pthread_mutex_lock(&data->mutex_isfinish);
				data->is_finish = 1;
				pthread_mutex_unlock(&data->mutex_isfinish);
				pthread_mutex_unlock(&data->mutex_last_time);
				return (NULL);
			}
			i++;
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
	while (i < data->num_of_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
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
	data->start_time = get_current_time();
	pthread_mutex_unlock(&data->mutex_start);
	pthread_create(&data->thread, NULL, monitor_thread, (void *)data);
}

#include <stdio.h>

void	print_data(t_data *data, char **argv)
{
	int	i;

	printf("Number of Philosophers: %d\n", data->num_of_philos);
	printf("Time to Die: %d ms\n", data->time_to_die);
	printf("Time to Eat: %d ms\n", data->time_to_eat);
	printf("Time to Sleep: %d ms\n", data->time_to_sleep);
	printf("Index: %d\n", data->index);
	printf("Is Finish: %d\n", data->is_finish);
	for (i = 0; i < data->num_of_philos; i++)
	{
		printf("\nPhilosopher %d:\n", data->philos[i].id);
		if (argv[5])
			printf("  Count Meal: %d\n", data->philos[i].count_meal);
		printf("  Last Time Eat: %ld ms\n", data->philos[i].last_time_eat);
	}
	printf("\n");
}

int	main(int argc, char **argv)
{
	t_data	*data;

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
	pthread_join(data->thread, NULL);
	cleanup(data);
	return (0);
}
