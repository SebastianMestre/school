# Informe

## .gitignore

El archivo .gitignore indica a git que archivos debe ignorar a la hora de ejecutar operaciones como git-add y git-status, entre otras.

En este caso, se le indica a git que ignore la carpeta vendor. esta carpeta contiene los archivos pertinetes a programas de terceros, como TravisCI.

## travis.yml

El archivo __'travis.yml'__ sirve para dar las especificaciones requeridas para poner travis en funcionamiento.

La linea:

```yml

language: php

```

especifica el lenguaje en el cual travis debera compilar los archivos especificados. Las lineas:

```yml

php:
  - 7.2

```

indican la version de php que se debera utilizar. Luego encontramos las lineas:

```yml

install:
  - composer update --prefer-source

  ```

`composer` es un administrador de dependencias, el cual se encarga de instalar dentro de nuestro proyecto las librerias requeridas. En el caso de estas depender de otras librerias, composer las instalara tambien.

`update` indica que deben actualizarse las dependencias y el archivo 'composer.lock' cada vez que el entorno PHP se actualice en travis (esto suele ser cada 30-60 dias).

`--prefer-source` se refiere a la fuente de donde seran instalados los respectivos paquetes. Al indicar 'source' como fuente, estos seran descargados desde el repositorio fuente. En este caso, este sera GitHub.

Por ultimo, las lineas:

```yml

script:
  - php vendor/bin/phpunit --color tests

  ```

Utilizamos `script` para indicar los comandos a utilizar cuando despleguemos nuestro proyecto. Estos estaran ubicados en `"vendor/bin/phpunit"`.

`--color` . . .

## composer.json y compose.lock

El archivo __'composer.json'__ describe las dependencias de nuestro proyecto. La clave `require` sera utilizada para este medio, indicando los paquetes correspondientes.

A diferencia de este ultimo, __'composer.lock'__ simplemente enlista todos los paquetes descargados y sus respectivas versiones. Es importante ya que de esta manera establece cuales son las versiones especificas ligadas a este proyecto. Debido a esto, es primordial añadir este archivo a nustro repositorio ya que de esta manera nos aseguramos de que todos los contribuidores del proyecto utilicen las versiones correctas.

## Alternativas a composer

Existen otras alternativas a composer, con soporte para otros lenguajes. Para __NodeJs__, el administrador de dependencias por defecto es __NPM__. A su vez, __Yarn__ ofrece soporte para NPM principalmente, aunque en teoria puede utilizarse con cualquier lenguaje. Por otro lado, __Bundle__ representa la opcion para __Ruby__.

## namespace

## {@inheritdoc}

## Herencia

Cuando una clase 'extiende' o 'hereda de' otra adquiere todos los miembros de esta. A la clase que hereda se le llama 'Sub-clase' y la clase de la cual hereda se le llama 'Super-clase'.

La herencia tiene varias aplicaciones y puede ayudarnos a implementar diversos mecanismos para abstraer nuestro codigo. En este caso, nos interesa el acceso a los metodos heredados como assertTrue y assertFalse, que son vitales para implementar nuestros unit tests.
