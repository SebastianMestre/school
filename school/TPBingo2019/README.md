# Trabajo Práctico Integrador - 2019

[![Build Status](https://travis-ci.org/sebastianmestre/TPBingo2019.svg?branch=master)](https://travis-ci.org/sebastianmestre/TPBingo2019)

![Selection_497](https://user-images.githubusercontent.com/14078528/58498174-e1148d80-8153-11e9-9c45-626c9a33858f.png)

## Consignas

### Escribir un informe respondiendo a las siguientes preguntas:

- ¿Para que sirve el archivo `.gitignore` incluido en el repositorio?. ¿Cuáles son sus limitaciones?
- ¿Para que sirve el archivo `.travis.yml`. Espeficique que hace cada linea del mismo.
- Para que sirve el archivo `composer.json` Que diferencia tiene con `composer.lock`. Como funciona el concepto de `psr-4` el archivo `composer.json`. ¿Que significa el concepto de `autoload`?
- Averigüe que alternativas para composer existen en NodeJS y Ruby existen.
- Qué función cumple la palabra `namespace` que aparece al principio de todos los archivos de las carpetas `src` y `tests` ¿que sucede si lo quitamos?
- Investigue que significa el comentario `{@inheritdoc}` que figura en los métodos de la clase `CartonJs` y `CartonEjemplo`.
- ¿Por que las clases del directorio `tests` extienden de la clase `TestCase`? ¿Qué significa que una clase extienda a otra clase?

### Código

- **Importante** Por ahora trabajar con el ejemplo de números provisto en el repositorio. No hay que hacer un generador de bingos automáticos.
- Realizar un fork de este repositorio.
- Conectar la ejecución de tests con travis. [Instrucciones](https://github.com/dagostinoips/TPBingo2019/wiki/Como-conectar-un-proyecto-con-travis)
- Completar los tests de la clase `tests/VerificacionesAvanzadasCartonTest.php`
- Verificar que pasen para la clase CartonEjemplo, pero fallen para la clase CartonJs.
- Luego de verificar que los tests fallen (con un commit), arreglar la clase CartonJs para que no falle más.
- Reemplazar la implementación de `lineas()` y `columnas()` para no repetir el código del constructor.


## Evaluación

El algoritmo que genera los números del CartonJs tiene algunas fallas. Una prueba de esto es lo que muestra el archivo index.php

```
[usuario@pc]$ php index.php

Intento 1:
[
  [ 8,  0,  0, 32, 44,  0, 61,  0, 86, ],
  [ 0,  0, 27,  0, 48, 59, 64, 74,  0, ],
  [ 7, 16, 20,  0,  0, 54,  0,  0, 84, ],
];

Intento 2:
[
  [ 0,  0, 24, 34, 46, 50,  0,  0, 80, ],
  [ 9, 17,  0, 33, 48,  0,  0, 79,  0, ],
  [ 0, 12,  0,  0,  0, 55, 67, 78, 90, ],
];

Intento 3:
[
  [ 0,  0, 24, 38,  0,  0, 60, 73, 89, ],
  [ 0, 15,  0, 35,  0, 55,  0, 77, 81, ],
  [ 6, 14, 22,  0, 47, 52,  0,  0,  0, ],
];

Intento 4:
[
  [ 7,  0, 23, 38,  0, 56, 65,  0,  0, ],
  [ 0, 14,  0, 34,  0, 52, 69,  0, 87, ],
  [ 1, 19,  0,  0, 41,  0,  0, 76, 86, ],
];

Intento 5:
[
  [ 0,  0, 26, 39, 48, 50,  0,  0, 86, ],
  [ 0, 12,  0,  0, 47, 54, 69,  0, 83, ],
  [ 4, 11, 22,  0,  0,  0, 68, 77,  0, ],
];
```

Como se puede ver, sólo los intentos 1 y 4 dieron cartones que se consideran _válidos_.

## Consignas de de la evaluación.

1. **Importante:** No se puede tocar el código del método FabricaCarton::intentoCarton.
1. Crear una clase nueva llamada Carton que acepte un cartón aleatorio en su constructor. Copiar los métodos filas, columnas y tieneNumero de la clase CartonEjemplo.
1. Borrar el test que valida que un cartón tiene un numero específico. (`VerificacionesBasicasCartonTest::testTieneNumero`)
1. La clase FabricaCarton debería contar con un nuevo método que se llame `validarCarton`. Que devuelva TRUE en caso de que el cartón sea válido.
1. La clase FabricaCarton deberia tener un nuevo método que devuelva cartones válidos. El código ya esta empezado pero tiene fallas que necesitan arreglo.
1. Modicar los test para que creen un cartón aleatorio usando la FabricaCarton.
1. La consigna es entonces generar un sistema que pruebe varias alternativas hasta que encuentre una válida. Y que corran los tests automaticos validando que el carton generado aleatoriamente efectivamente es válido.
