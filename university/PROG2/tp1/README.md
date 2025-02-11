Juego de Ahorcado
===============
Guia de uso.
---------------
- Al correr el programa deben ingresarse como parametros la ubicacion de los archivos **lemario** y **registro**, en ese orden.
- Si no se pasa ningun parametro, el programa terminara inmediatamente. Si solo se pasa un parametro, se asume que es el **lemario**.
- En tal caso, se creara un archivo **registro** en el directorio actual con el nombre _registro.txt_.
- El archivo **lemario** nunca puede estar vacio. En tal caso no existirian palabras para dar comienzo al juego. 
- Llegado el caso de que un jugador haya jugado con todas las palabras del **lemario**, _su registro se limpiara_ y se eligira una palabra cualquiera del **lemario**.

Detalles de implementacion.
---------------
- El **modelo de datos** utilizado esta especificado en el archivo _modelo.txt_.
- Para la implementacion del juego se utiliza el modulo _ahorcado_v1_ brindado por la catedra.
