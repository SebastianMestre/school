<?php

require 'vendor/autoload.php';

use Bingo\FabricaCartones;

$fabrica = new FabricaCartones;

foreach (range(1, 5) as $intento) {
  $carton = $fabrica->intentoCarton();
  print "Intento $intento:\n";
  imprimirCarton($carton);
}


function imprimirCarton($carton) {
  print("[\n");
  foreach (columnas($carton) as $columna) {
    print("  [");
    foreach ($columna as $celda) {
      printf("% 2d, ", $celda);
    }
    print("],");
    print("\n");
  }
  print("];\n\n");
}

function columnas(array $filas) {
  foreach ($filas as $indice_fila => $columna) {
    foreach ($columna as $indice_columna => $celda) {
      $columnas[$indice_columna][$indice_fila] = $celda;
    }
  }
  return $columnas;
}
