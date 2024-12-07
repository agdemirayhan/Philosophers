#include "philo.h"

t_data	*ft_get_data(void)
{
	static t_data	*data;

	if (!data)
	{
		data = malloc(sizeof(t_data));
		if (!data)
			return (NULL);
	}
	return (data);
}

long long	ft_get_time(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
	return ((time.tv_sec * 1000) + (time.tv_usec / 1000));
}

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


void	print_handler(t_data *data, t_philo *philo, int type)
{
	size_t	time;
	int		is_finish;
	int		count_meal;
	is_finish = data->is_finish;

	pthread_mutex_lock(&data->print);
	time = get_current_time() - data->start_time;
	// Only print if the simulation is not finished
	if (!data->is_finish)
	{
		if (type == 0) // Philosopher picks up forks and starts eating
			printf("%zu %d %s\n", time, philo->id, FORKS);

		else if (type == 1) // Philosopher is sleeping
			printf("%zu %d %s\n", time, philo->id, SLEEP);
		else if (type == 2) // Philosopher is thinking
			printf("%zu %d %s\n", time, philo->id, THINK);
		else if (type == 3) // Philosopher died
			printf("%zu %d %s\n", time, philo->id, DIED);
		else if( type==5)
			printf("%zu %d %s\n", time, philo->id, EAT);
	}

	else if (data->is_finish && type == 4)
	{
		printf("Philosophers ate %d times", data->count_meals);
	}
	pthread_mutex_unlock(&data->print);
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
		data->philos[i].count_meal = 0;
		data->philos[i].last_time_eat = 0;
		data->philos[i].left_fork = i;
		data->philos[i].right_fork = (i + 1) % data->num_of_philos;
		data->philos[i].data = data;
		pthread_mutex_init(&data->philos[i].lock, NULL);
		data->philos[i].last_time_eat = get_current_time();
	}
}

int	if_ended(t_data *events)
{
	int	ended;

	pthread_mutex_lock(events->finish);
	ended = events->is_finish;
	pthread_mutex_unlock(events->finish);
	return (ended);
}

long	curr_time(void)
{
	struct timeval  tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	print_action(t_data *events, int id, const char *action)
{
	pthread_mutex_lock(&events->print);
	printf("%ld %d %s\n", get_current_time() - events->start_time, id, action);
	pthread_mutex_unlock(&events->print);
}


void *philo_routine(void *arg)
{
	t_philo		*philo;
	t_data	*events;
	int			l_fork;
	int			r_fork;

	philo = (t_philo *)arg;
	events = philo->data;
	l_fork = philo->id - 1;
	r_fork = philo->id % events->num_of_philos;
	pthread_mutex_lock(&philo->lock);
	philo->last_time_eat = curr_time();
	pthread_mutex_unlock(&philo->lock);
	print_action(events, philo->id, "is thinking");
	if (philo->id % 2 == 0)
		ft_usleep(events->time_to_eat / 2);
	while (!events->is_finish)
	{
		pthread_mutex_lock(events->finish);
		if (events->is_finish || (events->count_meals != -1 && philo->count_meal >= events->count_meals))
		{
			pthread_mutex_unlock(events->finish);
			break ;
		}
		pthread_mutex_unlock(events->finish);
		if (l_fork < r_fork)
		{
			pthread_mutex_lock(events->forks[l_fork].lock);
			if (if_ended(events))
				break ;
			print_action(events, philo->id, "has taken a fork");
			if (events->num_of_philos == 1)
			{
				ft_usleep(events->time_to_die);
				return (pthread_mutex_unlock(events->forks[l_fork].lock), NULL);
			}
			pthread_mutex_lock(events->forks[r_fork].lock);
			if (if_ended(events))
				break ;
			print_action(events, philo->id, "has taken a fork");
		}
		else
		{
			pthread_mutex_lock(events->forks[r_fork].lock);
			if (if_ended(events))
				break ;
			print_action(events, philo->id, "has taken a fork");
			if (events->num_of_philos == 1)
			{
				ft_usleep(events->time_to_die);
				return (pthread_mutex_unlock(events->forks[l_fork].lock), NULL);
			}
			pthread_mutex_lock(events->forks[l_fork].lock);
			if (if_ended(events))
				break ;
			print_action(events, philo->id, "has taken a fork");
		}
		pthread_mutex_lock(&philo->lock);
		philo->last_time_eat = curr_time();
		print_action(events, philo->id, "is eating");
		philo->count_meal++;
		pthread_mutex_unlock(&philo->lock);
		if (philo->count_meal == events->count_meals)
		{
			pthread_mutex_lock(&events->meal);
			events->finished_philos++;
			pthread_mutex_unlock(&events->meal);
		}
		ft_usleep(events->time_to_eat);
		pthread_mutex_unlock(events->forks[r_fork].lock);
		pthread_mutex_unlock(events->forks[l_fork].lock);
		if (if_ended(events))
			break ;
		print_action(events, philo->id, "is sleeping");
		ft_usleep(events->time_to_sleep);
		if (if_ended(events))
			break ;
		print_action(events, philo->id, "is thinking");
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
			pthread_mutex_lock(&data->philos[i].lock);
			if (curr_time() - data->philos[i].last_time_eat > data->time_to_die)
			{
				print_handler(data, data->philos, 3);
				pthread_mutex_lock(data->finish);
				data->is_finish = 1;
				pthread_mutex_unlock(data->finish);
				pthread_mutex_unlock(&data->philos[i].lock);
				return (NULL);
			}
			if (data->count_meals != -1 && data->philos[i].count_meal < data->count_meals)
				mealcomplete = 0;
			pthread_mutex_unlock(&data->philos[i].lock);
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
				&data->philos[i]);
		i++;
	}
	data->start_time = curr_time();
	
	// Create the monitor thread
	pthread_create(&monitor, NULL, monitor_thread, data);
	pthread_join(monitor, NULL);
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
	if(!data)
		return -1;
	pthread_mutex_init(&data->meal, NULL);
	data->finish = 0;
	data->philos = (t_philo *)malloc((ft_atoi(argv[1]) * sizeof(t_philo)));
	if(!data->philos)
	{
		free(data->philos);
		free(data);
		return -1;		 
	}
	data->forks = (t_forks *)malloc(ft_atoi(argv[1]) * sizeof(t_forks));
	if(!data->forks)
	{
		free(data->forks);
		free(data);
		return -1;		 
	}
	pthread_mutex_init(&data->print, NULL);
	data->finish = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(data->finish, NULL);
	initialize(data, argv);
	if (argv[5])
		data->fifth_arg = 1;
	data->start_time = get_current_time();
	i = 0;
	while(i < data->num_of_philos)
	{
		data->forks[i].occ = 0;
		data->forks[i].lock = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(data->forks[i].lock, NULL);
		i++;
	}
	start_simulation(data);
	// Join all philosopher threads
	// Join the monitor thread
	cleanup(data);
	// print_data(data, argv);
	return (0);
}
