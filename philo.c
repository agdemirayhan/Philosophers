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
		
	}
	// Destroy shared mutexes in the data structure
	
	
	
	
	
	
	
	
	// Free allocated memory for philosophers
	free(data->philos);
}


void	print_handler(t_data *data, int type, int i)
{
	size_t	time;
	int		is_finish;
	int		count_meal;

	
	
	time = get_current_time() - data->start_time;
	
	
	is_finish = data->is_finish;
	
	// Lock before calculating time and printing
	time = get_current_time() - data->start_time;
	// Only print if the simulation is not finished
	if (!data->is_finish)
	{
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
	}
	else if (data->is_finish && type == 4)
	{
		printf("Philosophers ate %d times", data->count_meals);
		return ;
	}
	
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
	if (argv[5])
		data->count_meals = ft_atoi(argv[5]);

	while (++i < data->num_of_philos)
	{
		data->philos[i].id = i + 1;
		if (argv[5])
			data->philos[i].count_meal = ft_atoi(argv[5]);
		data->philos[i].last_time_eat = get_current_time();
		
	}
}

void	*philo_routine(void *args)
{
	t_data	*data;
	int		i;
	int		next;

	t_philo *philo = (t_philo *)args; 
	data = philo->data;
	
	i = philo->id - 1;

	next = (i + 1) % data->num_of_philos;


	if (data->num_of_philos == 1)
		return (NULL);

	if (data->num_of_philos != 1 && (i + 1) % 2 == 1)
	{
		print_handler(data, 2, i);
		ft_usleep(data->time_to_eat);
		// Initial staggered thinking for odd IDs
	}
	// Initialize last time eat

	philo->last_time_eat = get_current_time();


	while (1)
	{

		print_handler(data, 0, i); // Indicate fork-taking

		data->philos[i].last_time_eat = get_current_time();
		ft_usleep(data->time_to_eat); // Simulate eating
		// Update last time eat and decrement meal count if applicable
		if (philo->count_meal > 0)
		{
			philo->count_meal--;
			if (philo->count_meal == 0)
				data->finished_philos++;
		}

		// Put down forks

		// Sleep
		print_handler(data, 1, i);      // Indicate sleeping
		ft_usleep(data->time_to_sleep); // Simulate sleeping
		// Think
		print_handler(data, 2, i); // Indicate thinking
		if (data->is_finish)
		{
			return (NULL); // Exit if the simulation should stop
		}
	}
	return (NULL);
}

void	*monitor_thread(void *args)
{
	t_data	*data;
	int		i;
	size_t	current_time;
	int		count_meal;
	int		mealcomplete;

	data = (t_data *)args;
	while (1)
	{
		pthread_mutex_lock(data->finish);
		if (data->is_finish)
		{
			pthread_mutex_unlock(data->finish);
			break ;
		}
		pthread_mutex_unlock(data->finish);
		mealcomplete = 1;
		i = 0;
		while (i < data->num_of_philos)
		{
			pthread_mutex_lock(&data->philo[i].lock);
			if (get_current_time() - data->philo[i].last_time_eat > data->time_to_die)
			{
				print_handler(data, 3, i);
				pthread_mutex_lock(data->finish);
				data->is_finish = 1;
				pthread_mutex_unlock(data->finish);
				pthread_mutex_unlock(&data->philo[i].lock);
				return (NULL);
			}
			if (data->count_meals != -1 && data->philo[i].count_meal < data->count_meals)
				mealcomplete = 0;
			pthread_mutex_unlock(&data->philo[i].lock);
			i++;
		}
		if (mealcomplete && data->count_meals != -1)
		{
			pthread_mutex_lock(data->finish);
			data->is_finish = 1;
			pthread_mutex_unlock(data->finish);
			break ;
		}
		usleep(100);
	}
}

void	start_simulation(t_data *data)
{
	int	i;
	pthread_t	monitor;

	i = 0;
	
	while (i < data->num_of_philos)
	{
		data->philos[i].data = data;
		// Assign the shared data to each philosopher
		pthread_create(&data->philos[i].thread, NULL, philo_routine,
				(void *)&data->philos[i]);
		i++;
	}
	data->start_time = get_current_time();
	
	// Create the monitor thread
	pthread_create(&monitor, NULL, monitor_thread, data);
}

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
	cleanup(data);
	// print_data(data, argv);
	return (0);
}
