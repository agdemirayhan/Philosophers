/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aagdemir <aagdemir@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/12/08 18:22:48 by aagdemir          #+#    #+#             */
/*   Updated: 2024/12/08 19:16:51 by aagdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
