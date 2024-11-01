#include "philo.h"

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
			return ;
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
	return (arg);
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
	pthread_mutex_init(&params->dead_lock, NULL);
	pthread_mutex_init(&params->meal_lock, NULL);
	pthread_mutex_init(&params->write_lock, NULL);
	// Allocate memory for philosophers and fork mutexes
	if (!params->philos)
		return (0);
	i = 0;
	// Initialize fork mutexes
	while (i < num_of_philos)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	i = 0;
	while (i < num_of_philos)
	{
		// Initialize philosopher attributes
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
		// Set the mutex pointers for locking mechanisms
		params->philos[i].write_lock = &params->write_lock;
		params->philos[i].dead_lock = &params->dead_lock;
		params->philos[i].meal_lock = &params->meal_lock;
		// Set the forks for each philosopher
		params->philos[i].left_fork = &forks[i];
		params->philos[i].right_fork = &forks[(i + 1) % num_of_philos];
		// Associate the philosopher with the params structure
		params->philos[i].params = params;
		i++;
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
	pthread_t		monitor;
	int				i;
	pthread_mutex_t	*forks;

	forks = malloc(sizeof(pthread_mutex_t) * params->philos->num_of_philos);
	if (!forks)
	{
		free(params->philos);
		return (0);
	}
	if (pthread_create(&monitor, NULL, &monitor, params->philos) != 0)
		destory_all("Thread creation error", params, forks);
	i = 0;
	while (i < params->philos[0].num_of_philos)
	{
		if (pthread_create(&params->philos[i].thread, NULL, &philosopher_thread,
				&params->philos[i]) != 0)
			destory_all("Thread creation error", params, forks);
		i++;
	}
	i = 0;
	if (pthread_join(monitor, NULL) != 0)
		destory_all("Thread join error", params, forks);
	while (i < params->philos[0].num_of_philos)
	{
		if (pthread_join(params->philos[i].thread, NULL) != 0)
			destory_all("Thread join error", params, forks);
		i++;
	}
	return (0);
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
