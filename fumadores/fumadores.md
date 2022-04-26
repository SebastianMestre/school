## Respuestas a los apartados

### ¿Como puede ocurrir un deadlock?

Puede ocurrir un deadlock cuando dos fumadores toman un recurso al mismo tiempo.
Como los dos tienen un solo recurso no pueden empezar a fumar, pero el otro recurso no está disponible tampoco.
Entonces ninguno de los fumadores puede fumar.

### ¿Como funciona nuestra solución?

Nuestra solucion mueve la responsabilidad de conseguir los recursos desde los fumadores hacia la funcion *intentar_despertar_fumador*.
Esta función solo va a despertar a un fumador (usando semaforos) cuando los contadores de tabaco, papel y fosforos indiquen que el fumador va a tener todo lo necesario.

Para actualizar estos contadores creamos tres threads que esperan cada uno un recurso.
Cuando el recurso llega estos threads aumentan el contador correspondiente, llaman a *intentar_despertar_fumador* y vuelven a dormir.
Si las cantidades son adecuadas al llamar a la funcion, se va a despertar uno de los fumadores.
