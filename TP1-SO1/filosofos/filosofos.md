## ¿En que situación se puede producir un deadlock?

Se puede producir un deadlock si todos los filosofos toman el tenedor a su derecha al mismo tiempo.
Entonces todos los tenedores van a estar tomados pero ningun filósofo va a estar comiendo.
Si ningun filósofo come entonces ningún tenedor se va a liberar.

Los filósofos se quedan esperando por siempre a tener el otro tenedor.

## Explicación de la solución del filosofo zurdo

Estudiemos la situacion, centrandonos en el filosofo que esta a la derecha del filosofo zurdo.

Observemos que hay tres casos posibles:

1. El filosofo toma el tenedor de su derecha y luego el de su izquierda
2. El filosofo toma el tenedor de su derecha pero no puede tomar el de su izquierda
3. El filosofo no puede tomar el tenedor de su derecha

#### Caso 1

Si el filosofo toma el tenedor de su derecha y luego el de su izquierda, queda claro que puede comer.

#### Caso 2

Si el filosofo toma el tenedor de su derecha pero no puede tomar el de su izquierda, es porque el filosofo
zurdo (que esta a la izquierda de nuestro filosofo) ya lo tiene agarrado. Pero, al ser zurdo, este solo
tomaria el tenedor si ya tiene su tenedor izquierdo. Si asi fuera, podria comer.

#### Caso 3

Si el filosofo no puede tomar el tenedor de su derecha, es porque el filosofo de la derecha ya lo tiene
agarrado. Aparte, como este es diestro, ya debe haber tener su tenedor derecho. Entonces, este puede comer.

--------

Asi, concluimos que alguien puede comer en todos los casos. Por lo tanto, no hay deadlock.

## Explicación de la solución con semaforos

En esta solucion, siempre hay un filosofo que se abstiene. Fijemonos en el filosofo que se encuentra a la derecha de este.

Observemos que se pueden plantear tres casos:

1. El filosofo toma el tenedor de su derecha y luego el de su izquierda
2. El filosofo toma el tenedor de su derecha pero no puede tomar el de su izquierda
3. El filosofo no puede tomar el tenedor de su derecha

#### Caso 1

Si el filosofo toma el tenedor de su derecha y luego el de su izquierda, queda claro que puede comer.

#### Caso 2

Si el filosofo toma el tenedor de su derecha pero no puede tomar el de su izquierda, es porque el filosofo
que se abstiene (que esta a la izquierda de nuestro filosofo) ya lo tiene agarrado. Pero esto es absurdo,
asique este caso es imposible.

#### Caso 3

Si el filosofo no puede tomar el tenedor de su derecha, es porque el filosofo de la derecha ya lo tiene
agarrado. Aparte, como este es diestro, ya debe haber tener su tenedor derecho. Entonces, este puede comer.

--------

Asi, concluimos que alguien puede comer en todos los casos posibles. Por lo tanto, no hay deadlock.
