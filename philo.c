#include "philo.h"

// Example implementation of ft_usleep
void	ft_usleep(size_t time)
{
	usleep(time * 1000); // Convert milliseconds to microseconds
}

// Example implementation of get_current_time
size_t	get_current_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
	// Return time in milliseconds
}

// Example implementation of dead_loop
int	dead_loop(t_philo *philo)
{
	pthread_mutex_lock(philo->dead_lock);
	int dead = philo->params->dead_flag; // Access dead_flag from t_params
	pthread_mutex_unlock(philo->dead_lock);
	return (dead);
}

void	cleanup(t_params *params)
{
	// Destroy fork mutexes
	for (int i = 0; i < params->philos[0].num_of_philos; i++)
	{
		pthread_mutex_destroy(params->philos[i].left_fork);
	}
	// Destroy other mutexes
	pthread_mutex_destroy(&params->write_lock);
	pthread_mutex_destroy(&params->dead_lock);
	pthread_mutex_destroy(&params->meal_lock);
	// Free allocated memory
	free(params->philos[0].left_fork); // Free the forks
	free(params->philos);              // Free the philosophers
}

// Example implementation of destroy_all
void	destroy_all(const char *msg, t_params *params, pthread_mutex_t *forks)
{
	printf("%s\n", msg);
	cleanup(params);
	if (forks)
		free(forks);
	exit(1);
}

void	print_handler(char *str, t_philo *philo, int id)
{
	size_t	time;

	pthread_mutex_lock(philo->write_lock);
	time = get_current_time() - philo->start_time;
	if (!dead_loop(philo))
		printf("%zu %d %s\n", time, id, str);
	pthread_mutex_unlock(philo->write_lock);
}

long	current_time_ms(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
}

void	*philosopher_thread(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		ft_usleep(1);
	while (!dead_loop(philo))
	{
		// EAT LOOP
		pthread_mutex_lock(philo->right_fork);
		print_handler("has taken a fork", philo, philo->id);
		if (philo->num_of_philos == 1)
		{
			ft_usleep(philo->time_to_die);
			pthread_mutex_unlock(philo->right_fork);
			return ((void *)NULL);
		}
		pthread_mutex_lock(philo->left_fork);
		print_handler("has taken a fork", philo, philo->id);
		philo->eating = 1;
		print_handler("is eating", philo, philo->id);
		pthread_mutex_lock(philo->meal_lock);
		philo->last_meal = get_current_time();
		philo->meals_eaten++;
		pthread_mutex_unlock(philo->meal_lock);
		ft_usleep(philo->time_to_eat);
		philo->eating = 0;
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
		// SLEEP LOOP
		print_handler("is sleeping", philo, philo->id);
		ft_usleep(philo->time_to_sleep);
		print_handler("is thinking", philo, philo->id);
	}
	return ((void *)arg); // Return the argument or NULL
}

void	*monitor_thread(void *arg)
{
	t_params	*params;

	params = (t_params *)arg;
	size_t time_since_last_meal; // Declare as size_t to match time_to_die
	while (1)
	{
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
	int				i;

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
	params->philos = malloc(sizeof(t_philo) * num_of_philos);
	if (!params->philos)
		return (0);
	// Allocate memory for the fork mutexes
	forks = malloc(sizeof(pthread_mutex_t) * num_of_philos);
	if (!forks)
	{
		free(params->philos);
		return (0);
	}
	// Initialize mutexes
	pthread_mutex_init(&params->dead_lock, NULL);
	pthread_mutex_init(&params->meal_lock, NULL);
	pthread_mutex_init(&params->write_lock, NULL);
	// Initialize fork mutexes
	for (i = 0; i < num_of_philos; i++)
	{
		pthread_mutex_init(&forks[i], NULL);
	}
	// Initialize each philosopher
	for (i = 0; i < num_of_philos; i++)
	{
		params->philos[i].id = i + 1;
		params->philos[i].eating = 0;
		params->philos[i].meals_eaten = 0;
		params->philos[i].time_to_die = time_to_die;
		params->philos[i].time_to_eat = time_to_eat;
		params->philos[i].time_to_sleep = time_to_sleep;
		params->philos[i].start_time = current_time_ms();
		params->philos[i].last_meal = current_time_ms();
		params->philos[i].num_of_philos = num_of_philos;
		params->philos[i].num_times_to_eat = num_times_to_eat;
		params->philos[i].write_lock = &params->write_lock;
		params->philos[i].dead_lock = &params->dead_lock;
		params->philos[i].meal_lock = &params->meal_lock;
		params->philos[i].left_fork = &forks[i];
		params->philos[i].right_fork = &forks[(i + 1) % num_of_philos];
		params->philos[i].params = params;
	}
	return (1);
}

void	start_simulation(t_params *params)
{
	pthread_t	monitor;
	int			i;

	// Create the monitor thread
	if (pthread_create(&monitor, NULL, monitor_thread, (void *)params) != 0)
	{
		destroy_all("Thread creation error", params, NULL);
		return ;
	}
	// Create philosopher threads
	for (i = 0; i < params->philos[0].num_of_philos; i++)
	{
		if (pthread_create(&params->philos[i].thread, NULL, philosopher_thread,
				&params->philos[i]) != 0)
		{
			destroy_all("Thread creation error", params, NULL);
			return ;
		}
	}
	// Wait for the monitor thread to finish
	if (pthread_join(monitor, NULL) != 0)
		destroy_all("Thread join error", params, NULL);
	// Wait for all philosopher threads to finish
	for (i = 0; i < params->philos[0].num_of_philos; i++)
	{
		if (pthread_join(params->philos[i].thread, NULL) != 0)
			destroy_all("Thread join error", params, NULL);
	}
}

#include "philo.h"

int	main(int argc, char **argv)
{
	t_params	params;

	if (!initialize(&params, argc, argv))
		return (1);
	start_simulation(&params);
	cleanup(&params);
	return (0);
}
