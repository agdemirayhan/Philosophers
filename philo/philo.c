#include "philo.h"

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

// Example implementation of ft_usleep
int	ft_usleep(size_t milliseconds)
{
	size_t	start;

	start = get_current_time();
	while ((get_current_time() - start) < milliseconds)
		usleep(50);
	return (0);
}

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

void	print_handler(t_data *data, int type, int i)
{
	size_t	time;
	int		is_finish;

	pthread_mutex_lock(&data->mutex_time);
	time = get_current_time() - data->start_time;
	pthread_mutex_unlock(&data->mutex_time);
	pthread_mutex_lock(&data->mutex_isfinish);
	is_finish = data->is_finish;
	pthread_mutex_unlock(&data->mutex_isfinish);
	if (!is_finish)
	{
		pthread_mutex_lock(&data->mutex_print);
		if (type == 0)
			printf("%zu %d %s\n%zu %d %s\n%zu %d %s\n", time,
				data->philos[i].id, FORKS, time, data->philos[i].id, FORKS,
				time, data->philos[i].id, EAT);
		else if (type == 1)
			printf("%zu %d %s\n", time, data->philos[i].id, SLEEP);
		else if (type == 2)
			printf("%zu %d %s\n", time, data->philos[i].id, THINK);
		else if (type == 3)
			printf("%zu %d %s\n", time, data->philos[i].id, DIED);
		pthread_mutex_unlock(&data->mutex_print);
	}
}

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
	pthread_mutex_init(&(data->mutex_print), NULL);
	pthread_mutex_init(&(data->mutex_start), NULL);
	pthread_mutex_init(&(data->mutex_time), NULL);
	pthread_mutex_init(&(data->mutex_last_time), NULL);
	pthread_mutex_init(&(data->mutex_meal), NULL);
	pthread_mutex_init(&(data->mutex_index), NULL);
	pthread_mutex_init(&(data->mutex_thread), NULL);
	pthread_mutex_init(&(data->mutex_isfinish), NULL);
	while (++i < data->num_of_philos)
	{
		data->philos[i].id = i + 1;
		if (argv[5])
			data->philos[i].count_meal = ft_atoi(argv[5]);
		data->philos[i].last_time_eat = get_current_time();
		pthread_mutex_init(&data->philos[i].mutex_fork, NULL);
	}
}

#include <pthread.h>
#include <stdio.h>
#include <unistd.h> // For usleep

int	routine_loop(t_data *data, int i, int next)
{
	// Check if the simulation should stop
	pthread_mutex_lock(&data->mutex_isfinish);
	if (data->is_finish)
	{
		pthread_mutex_unlock(&data->mutex_isfinish);
		return (0);
	}
	pthread_mutex_unlock(&data->mutex_isfinish);
	// Take forks and eat
	pthread_mutex_lock(&data->philos[i].mutex_fork);
	pthread_mutex_lock(&data->philos[next].mutex_fork);
	print_handler(data, 0, i);
	ft_usleep(data->time_to_eat); // Simulate eating
	// Decrement the count of meals and check if this philosopher is done
	pthread_mutex_lock(&data->mutex_meal);
	if (data->philos[i].count_meal > 0)
	{
		data->philos[i].count_meal--;
		if (data->philos[i].count_meal == 0)
			data->finished_philos++;
	}
	pthread_mutex_unlock(&data->mutex_meal);
	// Put down forks
	pthread_mutex_unlock(&data->philos[next].mutex_fork);
	pthread_mutex_unlock(&data->philos[i].mutex_fork);
	// Sleep
	print_handler(data, 1, i);
	ft_usleep(data->time_to_sleep); // Simulate sleeping
	// Think
	print_handler(data, 2, i);
	// Check if the simulation should stop again
	pthread_mutex_lock(&data->mutex_isfinish);
	if (data->is_finish)
	{
		pthread_mutex_unlock(&data->mutex_isfinish);
		return (0);
	}
	pthread_mutex_unlock(&data->mutex_isfinish);
	return (1); // Continue the routine
}

void	*philo_routine(void *args)
{
	t_data	*data;
	int		i;
	int		next;

	t_philo *philo = (t_philo *)args; // Cast args to t_philo
	data = philo->data;
	// Access the shared data structure
	i = philo->id - 1;
	// Set i based on the philosopher's id
	next = (i + 1) % data->num_of_philos;
	// Calculate the next philosopher's index
	// Stagger start for philosophers with odd IDs (applies only at start)
	if (data->num_of_philos != 1 && (i + 1) % 2 == 1)
		ft_usleep(data->time_to_eat); // Initial staggered sleep for odd IDs
	// if (data->num_of_philos != 1 && data->philos[i].id % 2 == 1)
	// {
	// 	print_handler(data, 2, i);
	// 	ft_usleep(data->time_to_eat / 2);
	// }
	pthread_mutex_lock(&data->mutex_start);
	pthread_mutex_unlock(&data->mutex_start);
	// Initialize last time eat
	pthread_mutex_lock(&data->mutex_last_time);
	philo->last_time_eat = get_current_time();
	pthread_mutex_unlock(&data->mutex_last_time);
	// Main loop for the philosopher routine
	while (1)
	{
		// Check if the routine should stop before each loop
		pthread_mutex_lock(&data->mutex_isfinish);
		if (data->is_finish)
		{
			pthread_mutex_unlock(&data->mutex_isfinish);
			return (NULL); // Exit if the simulation should stop
		}
		pthread_mutex_unlock(&data->mutex_isfinish);
		// Take forks and eat
		pthread_mutex_lock(&data->philos[i].mutex_fork);
		pthread_mutex_lock(&data->philos[next].mutex_fork);
		print_handler(data, 0, i); // Indicate fork-taking
		pthread_mutex_lock(&data->mutex_last_time);
		data->philos[i].last_time_eat = get_current_time();
		pthread_mutex_unlock(&data->mutex_last_time);
		ft_usleep(data->time_to_eat); // Simulate eating
		// Update last time eat and decrement meal count if applicable
		pthread_mutex_lock(&data->mutex_last_time);
		philo->last_time_eat = get_current_time();
		pthread_mutex_unlock(&data->mutex_last_time);
		pthread_mutex_lock(&data->mutex_meal);
		if (philo->count_meal > 0)
		{
			philo->count_meal--;
			if (philo->count_meal == 0)
				data->finished_philos++;
		}
		pthread_mutex_unlock(&data->mutex_meal);
		// Put down forks
		pthread_mutex_unlock(&data->philos[next].mutex_fork);
		pthread_mutex_unlock(&data->philos[i].mutex_fork);
		// Sleep
		print_handler(data, 1, i);      // Indicate sleeping
		ft_usleep(data->time_to_sleep); // Simulate sleeping
		// Think
		print_handler(data, 2, i); // Indicate thinking
	}
	return (NULL);
}

void	*monitor_thread(void *args)
{
	t_data	*data;
	int		i;
	size_t	current_time;

	data = (t_data *)args;
	while (1)
	{
		// Check if all philosophers have finished eating
		pthread_mutex_lock(&data->mutex_meal);
		if (data->finished_philos == data->num_of_philos)
		{
			pthread_mutex_unlock(&data->mutex_meal);
			pthread_mutex_lock(&data->mutex_isfinish);
			data->is_finish = 1;
			pthread_mutex_unlock(&data->mutex_isfinish);
			break ;
		}
		pthread_mutex_unlock(&data->mutex_meal);
		// Check if any philosopher has died
		i = 0;
		while (i < data->num_of_philos)
		{
			// printf("\nPhilosopher %d:\n", data->philos[i].id);
			// printf("  Current Time: %ld ms\n", current_time);
			// printf("  Last Time Eat: %ld ms\n",
			// data->philos[i].last_time_eat);
			// printf("  Die checker: %zu ms\n", current_time
			// - data->philos[i].last_time_eat);
			pthread_mutex_lock(&data->mutex_last_time);
			current_time = get_current_time();
			if (current_time
				- data->philos[i].last_time_eat > (size_t)data->time_to_die)
			{
				print_handler(data, 3, i); // Use type 3 for "died" message
				pthread_mutex_lock(&data->mutex_isfinish);
				data->is_finish = 1;
				pthread_mutex_unlock(&data->mutex_isfinish);
				pthread_mutex_unlock(&data->mutex_last_time);
				return (NULL); // Exit the monitor thread
			}
			pthread_mutex_unlock(&data->mutex_last_time);
			i++;
		}
		ft_usleep(1); // Sleep for a short time to avoid busy-waiting
	}
	return (NULL);
}
void	start_simulation(t_data *data)
{
	int	i;

	i = 0;
	data->start_time = get_current_time();
	while (i < data->num_of_philos)
	{
		data->philos[i].data = data;
		// Assign the shared data to each philosopher
		pthread_create(&data->philos[i].thread, NULL, philo_routine,
			(void *)&data->philos[i]);
		i++;
	}
	// Create the monitor thread
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
	int		i;

	if (check_input(argc, argv) != 0)
	{
		printf("WRONG INPUTS!\n");
		return (WRONG_INPUT);
	}
	data = (t_data *)malloc(1 * sizeof(t_data));
	data->philos = (t_philo *)malloc((ft_atoi(argv[1]) * sizeof(t_philo)));
	initialize(data, argv);
	start_simulation(data);
	// Join all philosopher threads
	i = 0;
	while (i < data->num_of_philos)
	{
		pthread_join(data->philos[i].thread, NULL);
		i++;
	}
	// Join the monitor thread
	pthread_join(data->thread, NULL);
	printf("INPUTS ARE CORRECT!\n");
	// print_data(data, argv);
	return (0);
}
