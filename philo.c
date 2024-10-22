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
	while (1)
	{
		if (params->dead) // Stop if any philosopher has died
			break ;
		// Thinking
		pthread_mutex_lock(&params->print_mutex);
		printf("%ld %d is thinking\n", current_time_ms() - params->start_time,
			philo->id);
		pthread_mutex_unlock(&params->print_mutex);
		// Try to pick up forks (left and right)
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(&params->print_mutex);
		printf("%ld %d has taken a fork\n", current_time_ms()
			- params->start_time, philo->id);
		pthread_mutex_unlock(&params->print_mutex);
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(&params->print_mutex);
		printf("%ld %d has taken a fork\n", current_time_ms()
			- params->start_time, philo->id);
		pthread_mutex_unlock(&params->print_mutex);
		// Eating
		philo->last_meal_time = current_time_ms();
		pthread_mutex_lock(&params->print_mutex);
		printf("%ld %d is eating\n", current_time_ms() - params->start_time,
			philo->id);
		pthread_mutex_unlock(&params->print_mutex);
		usleep(params->time_to_eat * 1000);
		philo->meals_eaten++;
		// Put down the forks
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		// Sleeping
		pthread_mutex_lock(&params->print_mutex);
		printf("%ld %d is sleeping\n", current_time_ms() - params->start_time,
			philo->id);
		pthread_mutex_unlock(&params->print_mutex);
		usleep(params->time_to_sleep * 1000);
		if (params->dead) // Check again before continuing
			break ;
	}
	return (NULL);
}

void	*monitor_thread(void *arg)
{
	t_params	*params;
	long		time_since_last_meal;

	params = (t_params *)arg;
	while (1)
	{
		for (int i = 0; i < params->num_philos; i++)
		{
			time_since_last_meal = current_time_ms()
				- params->philosophers[i].last_meal_time;
			if (time_since_last_meal > params->time_to_die)
			{
				pthread_mutex_lock(&params->print_mutex);
				printf("%ld %d died\n", current_time_ms() - params->start_time,
					params->philosophers[i].id);
				pthread_mutex_unlock(&params->print_mutex);
				// Set the dead flag and stop the simulation
				params->dead = 1;
				return (NULL);
			}
		}
		usleep(1000); // Check every millisecond
		// If a philosopher has died, stop monitoring
		if (params->dead)
			break ;
	}
	return (NULL);
}

int	initialize(t_params *params, int argc, char **argv)
{
	// is_int logic must come here
	if (argc < 5 || argc > 6)
	{
		printf("Usage: ./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n");
		return (0);
	}
	params->num_philos = atoi(argv[1]);
	params->time_to_die = atoi(argv[2]);
	params->time_to_eat = atoi(argv[3]);
	params->time_to_sleep = atoi(argv[4]);
	if (argc == 6)
		params->meals_required = atoi(argv[5]);
	else
		params->meals_required = -1;
	// Initialize forks (mutexes)
	params->forks = malloc(sizeof(pthread_mutex_t) * params->num_philos);
	for (int i = 0; i < params->num_philos; i++)
		pthread_mutex_init(&params->forks[i], NULL);
	// Initialize philosophers
	params->philosophers = malloc(sizeof(t_philo) * params->num_philos);
	for (int i = 0; i < params->num_philos; i++)
	{
		params->philosophers[i].id = i + 1;
		params->philosophers[i].left_fork = &params->forks[i];
		params->philosophers[i].right_fork = &params->forks[(i + 1)
			% params->num_philos];
		params->philosophers[i].meals_eaten = 0;
		params->philosophers[i].last_meal_time = current_time_ms();
		params->philosophers[i].params = params;
	}
	// Initialize print mutex
	pthread_mutex_init(&params->print_mutex, NULL);
	// <-- This is the correct initialization
	return (1);
}

void	cleanup(t_params *params)
{
	for (int i = 0; i < params->num_philos; i++)
		pthread_mutex_destroy(&params->forks[i]);
	// Destroy the print mutex
	pthread_mutex_destroy(&params->print_mutex); // <-- Destroy the print mutex
	free(params->forks);
	free(params->philosophers);
}

void	start_simulation(t_params *params)
{
	pthread_t	monitor;

	params->start_time = current_time_ms();
	params->dead = 0; // Initialize the dead flag to 0 (no one is dead)
	// Create philosopher threads
	for (int i = 0; i < params->num_philos; i++)
		pthread_create(&params->philosophers[i].thread, NULL,
			philosopher_thread, &params->philosophers[i]);
	// Start the monitor thread
	pthread_create(&monitor, NULL, monitor_thread, params);
	// Wait for all philosopher threads to finish
	for (int i = 0; i < params->num_philos; i++)
		pthread_join(params->philosophers[i].thread, NULL);
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
