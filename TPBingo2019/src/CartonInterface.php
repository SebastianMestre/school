<?php

namespace Bingo;

interface CartonInterface {

  /**
   * Devuelve un array con las filas de un carton. Incluye valores vacios.
   * Cada fila es a su vez un array de enteros que representan los numeros de un
   * carton.
   */
  public function filas();

  /**
   * Devuelve un array con las columnas de un carton. Incluye valores vacios.
   * Cada columna es a su vez un array de enteros que representan los numeros de
   * uncarton.
   */
  public function columnas();

  /**
   * Una lista de los numeros de un carton. No incluye las celdas vacias.
   */
  public function numerosDelCarton();

  /**
   * Devuleve TRUE si el numero indicado es parte del carton.
   */
  public function tieneNumero(int $numero);

}
