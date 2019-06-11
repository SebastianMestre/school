<?php

namespace Bingo;

/**
 * Este Carton fue implementado durante la evaluacion.
 */
class Carton implements CartonInterface {

  private $numeros_carton = [];

  public function __construct(array $columnas) {
    foreach ($columnas as $indice_columna => $fila) {
      foreach ($fila as $indice_fila => $celda) {
        $filas[$indice_fila][$indice_columna] = $celda;
      }
    }
    
    $this->numeros_carton = $filas;
  }

  /**
   * {@inheritdoc}
   */
  public function filas() {
    return $this->numeros_carton;
  }

  /**
   * {@inheritdoc}
   */
  public function columnas() {
    // Nosotros metimos estos metodos en CartonJS
    $columnas_carton = [[]];
    foreach($this->numeros_carton as $fila){
      foreach($fila as $pos_celda => $celda){
        $columnas_carton[$pos_celda][] = $celda;
      }
    }
    return $columnas_carton;
  }

  /**
   * {@inheritdoc}
   */
  public function numerosDelCarton() {
    $numeros = [];
    foreach ($this->filas() as $fila) {
      foreach ($fila as $celda) {
        if ($celda != 0) {
          $numeros[] = $celda;
        }
      }
    }
    return $numeros;
  }

  /**
   * {@inheritdoc}
   */
  public function tieneNumero(int $numero) {
    return in_array($numero, $this->numerosDelCarton());
  }

}