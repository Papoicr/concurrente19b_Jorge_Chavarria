# Menor tiempo prime_hybrid_int
[B82073@arenal prime_hybrid_int]$ mpiexec -n 8 -f hosts.txt ./prime_hybrid_int 3 100000000
5761456 primes found in range [3, 100000000[ in 9.914407492s with 64



# Menor tiempo prime_process
[B82073@arenal prime_process]$ mpiexec -n 64 -f hosts.txt ./prime_process 3 100000000
5761462 primes found in range [3, 100000000[ in 14.579391003s with 64



