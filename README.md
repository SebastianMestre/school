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


