En un instituto sismográfico se tienen 40 sensores que recolectan y acumulan datos constantemente. Cada minuto envían un archivo de 1.5MB hacia los servidores centrales, donde 1MB=106B1MB=106B. Los sensores se turnan para enviar los datos con el fin de no saturar los servidores, y puede asumir que no ocurren colisiones de datos en la red entre los 40 sensores. Se tiene contratado un servicio de 16Mbps con el proveedor de servicio de Internet, por tanto, los servidores pueden recibir datos a esa velocidad. Se ha medido una latencia promedio de 0.025s porque hay sensores muy remotos. Se quieren instalar 30 sensores más ¿Se puede mantener el mismo ancho de banda con el proveedor de servicio o se debe aumentar? En caso de incrementarse ¿cuánto sería el ancho de banda mínimo que asegure que los datos se pueden recibir por los servidores centrales?

T = 0.025 + $\frac{1.5MB}{2MB}$ = 0.77

0.77*70 = 53.9

Como no se toma todo el minuto no se ocupa aumentar el ancho de banda
