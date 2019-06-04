# Informe

## .gitignore
El archivo .gitignore indica a git que archivos debe ignorar a la hora de ejecutar operaciones como git-add y git-status, entre otras.

En este caso, se le indica a git que ignore la carpeta vendor. esta carpeta contiene los archivos pertinetes a programas de terceros, como TravisCI.

## travis.yml

## composer.json y compose.lock

## Alternativas a composer

## namespace

## {@inheritdoc}

## Herencia

Cuando una clase 'extiende' o 'hereda de' otra adquiere todos los miembros de esta. A la clase que hereda se le llama 'Sub-clase' y la clase de la cual hereda se le llama 'Super-clase'.

La herencia tiene varias aplicaciones y puede ayudarnos a implementar diversos mecanismos para abstraer nuestro codigo. En este caso, nos interesa el acceso a los metodos heredados como assertTrue y assertFalse, que son vitales para implementar nuestros unit tests.
