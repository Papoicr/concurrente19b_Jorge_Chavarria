///implementacion mia
main:
	// How many threads have arrived to the barrier
	shared count := 0
	shared count2 := 0
	// Protects the increment of the count
	shared mutex := semaphore(1)
	shared mutex2 := semaphore(1)
	// Locked (0) until all threads arrive, then it is unlocked (1)
	shared turnstile := semaphore(0)
	shared barrier := semaphore(0)

	// Create arbitrary amount of threads
	shared thread_count := read_integer()
	create_thread(secondary, thread_count)

secondary:
	Statement A

	// Barrier
	wait(mutex)
	// If this is the last thread that reaches the barrier
	if ++count = thread_count:
		while count > 0:
			--count
			signal(turnstile)
	signal(mutex)
	wait(turnstile)

	// Statement B must be executed until
	// all threads have executed Statement A
	Statement B
	wait(mutex2)
	if ++count2 = thread_count:
		while count2 > 0:
			--count2
			signal(barrier)
	signal(mutex2)
	wait(barrier)
	
	
///Implementacion del profe (libro)
main:
	// How many threads have arrived to the barrier
	shared count := 0
	shared count2 := 0
	// Protects the increment of the count
	shared mutex := semaphore(1)
	shared mutex2 := semaphore(1)
	// Locked (0) until all threads arrive, then it is unlocked (1)
	shared turnstile := semaphore(0)
	shared barrier := semaphore(0)

	// Create arbitrary amount of threads
	shared thread_count := read_integer()
	create_thread(secondary, thread_count)
		
secondary:
	Statement A

	// Barrier
	wait(mutex)
	// If this is the last thread that reaches the barrier
	if ++count = thread_count:
		wait(barrier)
		signal(turnstile)
	signal(mutex)
	
	wait(turnstile)
	signal(turnstile)

	// Statement B must be executed until
	// all threads have executed Statement A
	Statement B
	wait(mutex)
	if --count == 0:
		wait(turnstile)
		signal(barrier)
	signal(mutex)
	
	wait(barrier)
	signal(barrier)
