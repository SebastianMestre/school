## Respuestas a los apartados

### ¿Por que el resultado dificilmente es 2\*N\_VISITANTES?

Esto se debe a que el valor de *visitantes* en muchas ocasiones es sobreescrito con un valor menor o igual al actual.

Al estar corriendo en simultaneo, un hilo puede demorarse mientras otro hilo avanza con su ejecucion.
Entonces el hilo que se demoro cuenta con un valor desactualizado de *visitantes* que luego incrementa y guarda.
Este valor que guarda resulta ser menor o igual que el valor actualizado de *visitantes*.

La siguiente trama ejemplifica esta situacion.
Luego de ejecutar una vez el codigo en dos hilos distintos la variable queda uno en vez de en dos.

```
[P1] int c1 = visitantes;
[P2] int c2 = visitantes;
[P1] c1 += 1;
[P1] visitantes = c1;
[P2] c2 += 1;
[P2] visitantes = c2;
```

### ¿El programa da el resultado correcto si N\_VISITANTES = 10?

El programa siempre imprime el  resultado correcto. Esto es así porque el proceso de levantar un thread se demora un tiempo.
Durante ese tiempo cada thread termina antes de que el siguiente thread empiece, por lo que no hay solapamiento.

### ¿Cual es el mínimo valor que podria imprimir el programa?

El valor mas bajo que puede imprimir es dos. Esto se consigue con la siguiente trama.

```
[P2] int c2 = visitantes;
...
 P1 corre 999999 veces
...
[P2] c2 += 1;
[P2] visitantes = c2; <----
[P1] int c1 = visitantes;
...
 P2 corre las restantes 999999 veces
...
[P1] c1 += 1;
[P1] visitantes = c1; <----
```

Al finalizar la trama, las unicas veces en las que *visitantes* fue realmente aumentado son en las lineas marcadas.
Todas las demas incrementaciones de *visitantes* se sobreescribieron por estas mismas lineas.
