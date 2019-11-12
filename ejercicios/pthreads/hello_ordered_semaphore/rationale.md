Al igual que con el arreglo de mutex, un arreglo de semaforos resueleve el problema de manera predecible por que se utiliza el id del thread como identificador 

*Diferencias*:
Al usar los semaforos hay que tener cuidado por que si se usa el id erroneo puede que 2 threads queden en la cola del mismo semforo, en pthreads si esto sucede los 2 threads se despiertan uno despues del otro, pero con semaforos se puede generar un deadlock.

El arreglo de mutex no debería funcionar por que la definición de un mutex dice que el thread no le puede hacer unlock a otro mutex mientras que con los semaforos esto si se permite.

