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
			
		}
		else if (type == 1)
			printf("%zu %d %s\n", time, data->philos[i].id, SLEEP);
		else if (type == 2)
			printf("%zu %d %s\n", time, data->philos[i].id, THINK);
		else if (type == 3)
			printf("%zu %d %s\n", time, data->philos[i].id, DIED);
		else if(type==4)
			printf("%zu %d %s\n", time, data->philos[i].id, EAT);
	}
	else if (data->is_finish && type == 4)
	{
		printf("Philosophers ate %d times", data->meal_limit);
		return ;
	}
}

void print_handler(t_data *data, int type, int i)
{
    size_t time;

    // Lock the individual philosopher's mutex
    pthread_mutex_lock(&data->philos[i].philo);

    // Calculate time since simulation start
    time = get_current_time() - data->start_time;

    // Unlock the individual philosopher's mutex
    pthread_mutex_unlock(&data->philos[i].philo);

    // Lock the global print mutex
    pthread_mutex_lock(&data->mutex_print);

    // Print the message using the helper function
    print_helper(data, type, i, time);

    // Unlock the global print mutex
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

void initialize(t_data *data, char **argv)
{
    int i;

    i = -1;
    data->num_of_philos = ft_atoi(argv[1]);
    data->time_to_die = ft_atoi(argv[2]);
    data->time_to_eat = ft_atoi(argv[3]);
    data->time_to_sleep = ft_atoi(argv[4]);
    data->index = 0;
    data->is_finish = 0;
    if (argv[5])
        data->meal_limit = ft_atoi(argv[5]);

    // Allocate memory for philos and forks arrays
    data->philos = malloc(sizeof(t_philo) * data->num_of_philos);
    if (!data->philos)
    {
        perror("Failed to allocate memory for philos");
        exit(EXIT_FAILURE);
    }

    data->forks = malloc(sizeof(t_forks) * data->num_of_philos);
    if (!data->forks)
    {
        perror("Failed to allocate memory for forks");
        free(data->philos);
        exit(EXIT_FAILURE);
    }

    // Initialize philos and forks
    while (++i < data->num_of_philos)
    {
        data->philos[i].id = i + 1;
        if (argv[5])
            data->philos[i].count_meal = ft_atoi(argv[5]);
        data->philos[i].last_time_eat = get_current_time();
        
        // Allocate and initialize the fork mutex
        data->forks[i].lock_fork = malloc(sizeof(pthread_mutex_t));
        if (!data->forks[i].lock_fork)
        {
            perror("Failed to allocate memory for fork mutex");
            free(data->philos);
            free(data->forks);
            exit(EXIT_FAILURE);
        }
        pthread_mutex_init(data->forks[i].lock_fork, NULL);
    }
}

