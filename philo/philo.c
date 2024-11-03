#include "philo.h"

// Example implementation of ft_usleep
// void	ft_usleep(size_t time)
// {
// }

// // Example implementation of get_current_time
// size_t	get_current_time(void)
// {
// }

// void	cleanup(t_params *params)
// {
// }

// // Example implementation of destroy_all
// void	destroy_all(char *str, t_params *params, pthread_mutex_t *forks)
// {
// }

// void	print_handler(char *str, t_philo *philo, int id)
// {
// }

// void	*philosopher_thread(void *arg)
// {
// }

// void	print_message(char *str, t_philo *philo, int id)
// {
// }

// // Monitor thread routine

// void	*monitor_thread(void *pointer)
// {
// }

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
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
	while (++i < data->num_of_philos)
	{
		data->philos[i].id = i + 1;
		if (argv[5])
			data->philos[i].count_meal = ft_atoi(argv[5]);
		data->philos[i].last_time_eat = get_current_time();
		pthread_mutex_init(&data->philos[i].mutex_fork, NULL);
	}
}

// void	start_simulation(t_params *params)
// {
// }

#include <stdio.h>

void	print_data(t_data *data,char **argv)
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
		if(argv[5])
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
		printf("WRONG INPUTS!");
		return (WRONG_INPUT);
	}
	data = (t_data *)malloc(1 * sizeof(t_data));
	data->philos = (t_philo *)malloc((ft_atoi(argv[1]) * sizeof(t_philo)));
	initialize(data, argv);
	printf("INPUTS ARE CORRECT!");
	print_data(data,argv);
	return (0);
}

/////////////////////////////////////
// TEST

// #include <pthread.h>
// #include <stdio.h> // printf
// #include <stdlib.h> //malloc
// #include <unistd.h>

// typedef struct data_s
// {
// 	int				num;
// 	pthread_mutex_t	mutex;
// }					data_t;

// void exit_on_error(data_t *data, int exit_code)
// {
// 	free(data);
// 	exit(exit_code);
// }

// void *routine_1(void *arg)
// {
// 	data_t *data;
// 	int i;

// 	data = (data_t *)arg;
// 	i = 0;
// 	while(i<10000000)
// 	{
// 		pthread_mutex_lock(&data->mutex);
// 		(data->num)++;
// 		pthread_mutex_unlock(&data->mutex);
// 		i++;

// 	}
// 	return (NULL);
// }

// void	example(void)
// {
// 	data_t		*data;
// 	pthread_t	t1;
// 	pthread_t	t2;
// 	pthread_t	t3;
// 	pthread_t	t4;

// 	data = malloc(sizeof(data_t));
// 	if(data == NULL)
// 		exit(1);
// 	data->num = 0;
// 	pthread_mutex_init(&data->mutex, NULL);

// 	if(pthread_create(&t1,NULL,routine_1,data))
// 		exit_on_error(data,2);
// 	if(pthread_create(&t2,NULL,routine_1,data))
// 		exit_on_error(data,2);
// 	if(pthread_create(&t3,NULL,routine_1,data))
// 		exit_on_error(data,2);
// 	if(pthread_create(&t4,NULL,routine_1,data))
// 		exit_on_error(data,2);

// 	if(pthread_join(t1,NULL))
// 		exit_on_error(data,3);
// 	// if(pthread_join(t2,NULL))
// 	// 	exit_on_error(data,3);
// 	// if(pthread_join(t3,NULL))
// 	// 	exit_on_error(data,3);
// 	// if(pthread_join(t4,NULL))
// 	// 	exit_on_error(data,3);

// 	printf("Num: %d\n", data->num);
// 	pthread_mutex_destroy(&data->mutex);
// 	free(data);

// }

// int	main(int argc, char **argv)
// {
// 	example();
// 	return (0);
// }
