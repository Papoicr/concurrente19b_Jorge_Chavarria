Un equipo de investigadores necesita hacer un análisis de 7.2TB de datos, donde 1TB=1012B1TB=1012B. Dado que el análisis tomará mucho tiempo, se ha decidido usar un laboratorio de 28 computadoras que están conectadas por un _switch_ de Ethernet con un ancho de banda de 100Mbps y una latencia promedio de 20ms. Restando el ancho de banda extra para "enrutar" y corregir errores (_overhead_), suponga que la red puede transmitir datos a 85Mbps. Los datos están alojados en una de las 28 computadoras de la red y usted debe tratar de distribuir cantidades iguales de datos a cada una de las otras computadoras del laboratorio. Los datos están en archivos de 1MB cada uno. Los investigadores quieren saber ¿cuánto tiempo tardará en estar listos los datos para iniciar el análisis en el laboratorio?  

Se saca cuantos archivos se le reparten a cada computadora

$\frac{7200000}{28}$ = 257142 y el residuo es 24, este 24 se ditribuye en 24 computadoras

Se le resta ese numero a las 7,2 TB ya que una computadora ya tiene los datos lo que genera la formula

L = 0.02s + $\frac{6942857MB}{10,625MB/s}$ 

L = 653445,8547s aprox 181 horas
