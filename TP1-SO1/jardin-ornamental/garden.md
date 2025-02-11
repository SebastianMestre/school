### ¿Por que el resultado dificilmente es 2\*N\_VISITANTES?

Esto se debe a que la operacion de incremento no es atomica.

Al no ser atomica, puede pasar que un hilo lea el valor de visitantes, que el otro hilo lea el mismo valor, opere,
y guarde un nuevo valor, y que el primer hilo finalmente opere y guarde un valor que no contempla las operaciones
realizadas por el segundo. Entonces, el valor de *visitantes* en muchas ocasiones es sobreescrito con un valor menor
o igual al esperado.

La siguiente traza ejemplifica esta situacion. Luego de ejecutar una vez el codigo en dos hilos distintos, la variable
tiene un uno en vez de un dos.

```
[P1] int c1 = visitantes; <---- Lee 0
[P2] int c2 = visitantes; <---- Lee 0
[P1] c1 += 1;
[P2] c2 += 1;
[P1] visitantes = c1;     <---- Guarda 1
[P2] visitantes = c2;     <---- Guarda 1
```

### ¿El programa da el resultado correcto si N\_VISITANTES = 10?

En nuestras pruebas, el programa siempre imprime el resultado correcto. Esto es así porque el proceso de levantar un
thread toma tiempo. Durante ese tiempo, cada thread termina antes de que el siguiente thread empiece, por lo
que no hay solapamiento.

### ¿Cual es el mínimo valor que podria imprimir el programa?

El valor mas bajo que puede imprimir es dos. Esto se consigue con la siguiente traza.

```
[P2] int c2 = visitantes; <---- Lee 0
...
 P1 corre 999999 veces
...
[P2] c2 += 1;
[P2] visitantes = c2;     <---- Guarda 1
[P1] int c1 = visitantes; <---- Lee 1
...
 P2 corre las 999999 veces restantes
...
[P1] c1 += 1;
[P1] visitantes = c1;     <---- Guarda 2
```

Al finalizar la traza, las unicas veces en las que *visitantes* fue realmente aumentado son en las lineas marcadas.
Todos los demas incrementos de *visitantes* se sobreescriben en esos puntos de la traza.
