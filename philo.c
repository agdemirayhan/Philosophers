#include "philo.h"

long	current_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}
void	*philosopher_thread(void *arg)
{
	t_philo		*philo;
	t_params	*params;

	philo = (t_philo *)arg;
	params = philo->params;
	// Handle the special case where there's only one philosopher
	if (philo->num_of_philos == 1)
	{
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->write_lock);
		printf("%ld %d has taken a fork\n", current_time_ms()
			- philo->start_time, philo->id);
		pthread_mutex_unlock(philo->write_lock);
		usleep(philo->time_to_die * 1000);
		pthread_mutex_lock(philo->write_lock);
		printf("%ld %d died\n", current_time_ms() - philo->start_time,
			philo->id);
		params->dead_flag = 1;
		pthread_mutex_unlock(philo->write_lock);
		pthread_mutex_unlock(philo->left_fork);
		return (NULL);
	}
	// Normal case for more than one philosopher
	while (1)
	{
		// Check if the simulation has ended
		pthread_mutex_lock(philo->dead_lock);
		if (params->dead_flag)
		{
			pthread_mutex_unlock(philo->dead_lock);
			break ;
		}
		pthread_mutex_unlock(philo->dead_lock);
		// Try to pick up forks
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->write_lock);
		printf("%ld %d has taken a fork\n", current_time_ms()
			- philo->start_time, philo->id);
		pthread_mutex_unlock(philo->write_lock);
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(philo->write_lock);
		printf("%ld %d has taken a fork\n", current_time_ms()
			- philo->start_time, philo->id);
		pthread_mutex_unlock(philo->write_lock);
		// Eating
		pthread_mutex_lock(philo->meal_lock);
		philo->eating = 1;
		philo->last_meal = current_time_ms();
		pthread_mutex_unlock(philo->meal_lock);
		pthread_mutex_lock(philo->write_lock);
		printf("%ld %d is eating\n", current_time_ms() - philo->start_time,
			philo->id);
		pthread_mutex_unlock(philo->write_lock);
		usleep(philo->time_to_eat * 1000);
		philo->meals_eaten++;
		philo->eating = 0;
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		// Check if the philosopher has eaten enough times
		if (philo->num_times_to_eat != -1
			&& philo->meals_eaten >= philo->num_times_to_eat)
			break ;
		// Thinking
		pthread_mutex_lock(philo->write_lock);
		printf("%ld %d is thinking\n", current_time_ms() - philo->start_time,
			philo->id);
		pthread_mutex_unlock(philo->write_lock);
		usleep(philo->time_to_sleep * 1000);
	}
	return (NULL);
}
void	*monitor_thread(void *arg)
{
	t_params	*params;

	params = (t_params *)arg;
	size_t time_since_last_meal; // Declare as size_t to match time_to_die
	while (1)
	{
		// Check if all philosophers have finished eating the required number of meals
		pthread_mutex_lock(&params->meal_lock);
		if (params->philos_done == params->philos[0].num_of_philos)
		{
			pthread_mutex_lock(&params->write_lock);
			printf("All philosophers have finished eating. Stopping simulation.\n");
			pthread_mutex_unlock(&params->write_lock);
			pthread_mutex_lock(&params->dead_lock);
			params->dead_flag = 1; // Signal to stop the simulation
			pthread_mutex_unlock(&params->dead_lock);
			pthread_mutex_unlock(&params->meal_lock);
			return (NULL);
		}
		pthread_mutex_unlock(&params->meal_lock);
		// Check if any philosopher has died
		for (int i = 0; i < params->philos[0].num_of_philos; i++)
		{
			pthread_mutex_lock(&params->meal_lock);
			time_since_last_meal = current_time_ms()
				- params->philos[i].last_meal;
			pthread_mutex_unlock(&params->meal_lock);
			if (time_since_last_meal > params->philos[i].time_to_die)
			{
				pthread_mutex_lock(&params->write_lock);
				printf("%ld %d died\n", current_time_ms()
					- params->philos[i].start_time, params->philos[i].id);
				pthread_mutex_unlock(&params->write_lock);
				pthread_mutex_lock(&params->dead_lock);
				params->dead_flag = 1; // Signal to stop the simulation
				pthread_mutex_unlock(&params->dead_lock);
				return (NULL);
			}
		}
		usleep(1000); // Check every millisecond
	}
	return (NULL);
}

int	initialize(t_params *params, int argc, char **argv)
{
	int				num_of_philos;
	size_t			time_to_die;
	size_t			time_to_eat;
	size_t			time_to_sleep;
	int				num_times_to_eat;
	pthread_mutex_t	*forks;

	if (argc < 5 || argc > 6)
	{
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (0);
	}
	// Parse arguments
	num_of_philos = atoi(argv[1]);
	time_to_die = (size_t)atoi(argv[2]);
	time_to_eat = (size_t)atoi(argv[3]);
	time_to_sleep = (size_t)atoi(argv[4]);
	num_times_to_eat = (argc == 6) ? atoi(argv[5]) : -1;
	// Initialize params
	params->dead_flag = 0;
	pthread_mutex_init(&params->dead_lock, NULL);
	pthread_mutex_init(&params->meal_lock, NULL);
	pthread_mutex_init(&params->write_lock, NULL);
	// Allocate memory for philosophers and fork mutexes
	params->philos = malloc(sizeof(t_philo) * num_of_philos);
	if (!params->philos)
		return (0);
	forks = malloc(sizeof(pthread_mutex_t) * num_of_philos);
	if (!forks)
	{
		free(params->philos);
		return (0);
	}
	// Initialize fork mutexes
	for (int i = 0; i < num_of_philos; i++)
		pthread_mutex_init(&forks[i], NULL);
	// Initialize each philosopher
	for (int i = 0; i < num_of_philos; i++)
	{
		params->philos[i].id = i + 1;
		params->philos[i].eating = 0;
		params->philos[i].meals_eaten = 0;
		params->philos[i].last_meal = current_time_ms();
		params->philos[i].time_to_die = time_to_die;
		params->philos[i].time_to_eat = time_to_eat;
		params->philos[i].time_to_sleep = time_to_sleep;
		params->philos[i].start_time = current_time_ms();
		params->philos[i].num_of_philos = num_of_philos;
		params->philos[i].num_times_to_eat = num_times_to_eat;
		params->philos[i].left_fork = &forks[i];
		params->philos[i].right_fork = &forks[(i + 1) % num_of_philos];
		params->philos[i].write_lock = &params->write_lock;
		params->philos[i].dead_lock = &params->dead_lock;
		params->philos[i].meal_lock = &params->meal_lock;
		params->philos[i].params = params;
	}
	return (1);
}

void	cleanup(t_params *params)
{
	// Destroy fork mutexes
	for (int i = 0; i < params->philos[0].num_of_philos; i++)
		pthread_mutex_destroy(params->philos[i].left_fork);
	// Destroy other mutexes
	pthread_mutex_destroy(&params->write_lock);
	pthread_mutex_destroy(&params->dead_lock);
	pthread_mutex_destroy(&params->meal_lock);
	// Free allocated memory
	free(params->philos[0].left_fork); // Free the forks
	free(params->philos);              // Free the philosophers
}

void	start_simulation(t_params *params)
{
	pthread_t	monitor;

	params->philos[0].start_time = current_time_ms();
	// Create philosopher threads
	for (int i = 0; i < params->philos[0].num_of_philos; i++)
		pthread_create(&params->philos[i].thread, NULL, philosopher_thread,
			&params->philos[i]);
	// Start the monitor thread
	pthread_create(&monitor, NULL, monitor_thread, params);
	// Wait for all philosopher threads to finish
	for (int i = 0; i < params->philos[0].num_of_philos; i++)
		pthread_join(params->philos[i].thread, NULL);
	// Wait for the monitor thread to finish
	pthread_join(monitor, NULL);
}

int	main(int argc, char **argv)
{
	t_params	params;

	if (!initialize(&params, argc, argv))
		return (1);
	start_simulation(&params);
	cleanup(&params);
	return (0);
}
