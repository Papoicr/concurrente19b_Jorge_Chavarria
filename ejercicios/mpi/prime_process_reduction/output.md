# Menor tiempo prime_hybrid_int
[B82073@arenal prime_hybrid_int]$ mpiexec -n 637 -f hosts.txt ./prime_hybrid_int 3 100000000
5761520 primes found in range [3, 100000000[ in 11.961823940s with 5096




# Menor tiempo prime_process
[B82073@arenal prime_process]$ mpiexec -n 637 -f hosts.txt ./prime_process 3 100000000
5761520 primes found in range [3, 100000000[ in 14.523048162s with 637

# Menor tiempo prime_process_reduction
[B82073@arenal prime_process_reduction]$ mpiexec -n 637 -f hosts.txt ./prime_process_reduction 3 100000000
5761520 primes found in range [3, 100000000[ in 14.622192621s with 637

El menor tiempo lo da hybrid_int


