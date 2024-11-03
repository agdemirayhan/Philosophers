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

// int	initialize(t_params *params, int argc, char **argv)
// {
// }

// void	start_simulation(t_params *params)
// {
// }

long	ft_atoi(const char *str)
{
	long	sign;
	long	ab_num;

	sign = 1;
	ab_num = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign *= -1;
		str++;
	}
	while (*str == '0')
		str++;
	while (*str >= '0' && *str <= '9')
	{
		ab_num = 10 * ab_num + (*str - '0');
		str++;
	}
	return (ab_num * sign);
}

int	is_pos_digit(int argc, char **argv)
{
	int i = 1;
	while(i < argc)
	{
		if(ft_atoi(argv[i]) <= 0)
			return WRONG_INPUT;
		i++;
	}
	return 0;
}

int	check_input(int argc, char **argv)
{
	if (argc < 5 || argc > 6)
		return (WRONG_INPUT);
	if (is_pos_digit(argc, argv) != 0)
		return (WRONG_INPUT);
	return 0;
}

int	main(int argc, char **argv)
{
	if (check_input(argc, argv) != 0)
	{
		printf("WRONG INPUTS!");
		return (WRONG_INPUT);
	}
	printf("INPUTS ARE CORRECT!");
	return 0;
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
