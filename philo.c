#include "philo.h"

size_t	get_current_time(void)
{
	struct timeval	time;

	if (gettimeofday(&time, NULL) == -1)
		write(2, "gettimeofday() error\n", 22);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

// Example implementation of ft_usleep
void	ft_usleep(size_t milliseconds)
{
	size_t	start;

	start = get_current_time();
	while ((get_current_time() - start) < milliseconds)
		usleep(50);
}

void	cleanup(t_data *data)
{
	// Destroy mutexes associated with each philosopher's fork
	for (int i = 0; i < data->num_of_philos; i++)
	{
		pthread_mutex_destroy(&data->philos[i].mutex_fork);
	}
	// Destroy shared mutexes in the data structure
	pthread_mutex_destroy(&data->mutex_print);
	pthread_mutex_destroy(&data->mutex_start);
	pthread_mutex_destroy(&data->mutex_time);
	pthread_mutex_destroy(&data->mutex_last_time);
	pthread_mutex_destroy(&data->mutex_meal);
	pthread_mutex_destroy(&data->mutex_index);
	pthread_mutex_destroy(&data->mutex_thread);
	pthread_mutex_destroy(&data->mutex_isfinish);
	// Free allocated memory for philosophers
	free(data->philos);
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

void print_handler(t_data *data, int type, int i)
{
    size_t time;

    // Lock the print mutex to ensure thread-safe printing
    pthread_mutex_lock(&data->mutex_print);

    // Lock the time mutex to read the current time safely
    pthread_mutex_lock(&data->mutex_time);
    time = get_current_time() - data->start_time;
    pthread_mutex_unlock(&data->mutex_time);

    // Check if the simulation is finished
    pthread_mutex_lock(&data->mutex_isfinish);
    if (data->is_finish)
    {
        pthread_mutex_unlock(&data->mutex_isfinish);
        if (type == 4) // Handle case where type == 4
        {
            printf("Philosophers ate %d times\n", data->count_meals);
        }
        pthread_mutex_unlock(&data->mutex_print); // Ensure mutex is unlocked in all cases
        return;
    }
    pthread_mutex_unlock(&data->mutex_isfinish);

    // Print messages based on the type
    if (type == 0) // Philosopher picks up forks and starts eating
    {
        printf("%zu %d %s\n", time, data->philos[i].id, FORKS);
        printf("%zu %d %s\n", time, data->philos[i].id, FORKS);
        printf("%zu %d %s\n", time, data->philos[i].id, EAT);
    }
    else if (type == 1) // Philosopher is sleeping
        printf("%zu %d %s\n", time, data->philos[i].id, SLEEP);
    else if (type == 2) // Philosopher is thinking
        printf("%zu %d %s\n", time, data->philos[i].id, THINK);
    else if (type == 3) // Philosopher died
        printf("%zu %d %s\n", time, data->philos[i].id, DIED);
    pthread_mutex_unlock(&data->mutex_print); // Unlock the print mutex after printing
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
	if (argv[5])
		data->count_meals = ft_atoi(argv[5]);
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

void	*philo_routine(void *args)
{
	t_philo	*philo = (t_philo *)args;
	t_data	*data = philo->data;
	int		i = philo->id - 1;
	int		next = (i + 1) % data->num_of_philos;

	// Handle the case where there is only one philosopher
	pthread_mutex_lock(&data->mutex_start); // Wait for start signal
    pthread_mutex_unlock(&data->mutex_start);
	if (data->num_of_philos == 1)
	{
		pthread_mutex_lock(&data->philos[i].mutex_fork);
		ft_usleep(data->time_to_die);
		pthread_mutex_unlock(&data->philos[i].mutex_fork);
		return (NULL);
	}

	// Delay start for even philosophers to avoid immediate contention
	if (i % 2 == 1)
		ft_usleep(data->time_to_eat / 2);

	while (1)
	{
		// Lock forks in consistent order
		if (i < next)
		{
			pthread_mutex_lock(&data->philos[i].mutex_fork); // Lock lower index first
			pthread_mutex_lock(&data->philos[next].mutex_fork);
		}
		else
		{
			pthread_mutex_lock(&data->philos[next].mutex_fork); // Lock lower index first
			pthread_mutex_lock(&data->philos[i].mutex_fork);
		}

		// Start eating
		print_handler(data, 0, i);
		pthread_mutex_lock(&data->mutex_last_time);
		philo->last_time_eat = get_current_time();
		pthread_mutex_unlock(&data->mutex_last_time);
		ft_usleep(data->time_to_eat);

		// Track meals
		pthread_mutex_lock(&data->mutex_meal);
		if (philo->count_meal > 0)
		{
			philo->count_meal--;
			if (philo->count_meal == 0)
				data->finished_philos++;
		}
		pthread_mutex_unlock(&data->mutex_meal);

		// Unlock forks
		pthread_mutex_unlock(&data->philos[i].mutex_fork);
		pthread_mutex_unlock(&data->philos[next].mutex_fork);

		// Start sleeping
		print_handler(data, 1, i);
		ft_usleep(data->time_to_sleep);

		// Start thinking
		print_handler(data, 2, i);

		// Check if simulation should finish
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
			if ((current_time
					- data->philos[i].last_time_eat > (size_t)data->time_to_die)
				&& current_time > data->philos[i].last_time_eat)
			{
				// printf("check death:%lu\n", current_time
				// 		- data->philos[i].last_time_eat);
				// printf("current_time:%lu\n", current_time);
				// printf("lasttime eat:%zu\n", data->philos[i].last_time_eat);
				print_handler(data, 3, i); // Use type 3 for "died" message
				pthread_mutex_lock(&data->mutex_isfinish);
				data->is_finish = 1;
				pthread_mutex_unlock(&data->mutex_isfinish);
				pthread_mutex_unlock(&data->mutex_last_time);
				return (NULL); // Exit the monitor thread
			}
			i++;
			pthread_mutex_unlock(&data->mutex_last_time);
		}
		i = 0;
		// printf("TEST1");
		while (i < data->num_of_philos && data->fifth_arg)
		{
			// printf("philo count_meal:%d\n", data->philos[i].count_meal);
			pthread_mutex_lock(&data->mutex_meal);
			count_meal = data->philos[i].count_meal;
			pthread_mutex_unlock(&data->mutex_meal);
			// printf("TEST3");
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
		// printf("TEST2");
	}
	return (NULL);
}
void	start_simulation(t_data *data)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&data->mutex_start); // Block threads from starting immediately
    for (int i = 0; i < data->num_of_philos; i++) {
        data->philos[i].data = data;
        pthread_create(&data->philos[i].thread, NULL, philo_routine, (void *)&data->philos[i]);
    }
    pthread_mutex_lock(&data->mutex_time);
    data->start_time = get_current_time(); // Initialize start_time safely
    pthread_mutex_unlock(&data->mutex_time);
    pthread_mutex_unlock(&data->mutex_start); // Allow threads to proceed
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
	if (argv[5])
		data->fifth_arg = 1;
	start_simulation(data);
	// Join all philosopher threads
	// Join the monitor thread
	for (int i = 0; i < data->num_of_philos; i++) 
	{
   		pthread_join(data->philos[i].thread, NULL);
	}
	pthread_join(data->thread, NULL);
	cleanup(data);
	// print_data(data, argv);
	return (0);
}
