<?php

namespace Bingo;

use PHPUnit\Framework\TestCase;

class VerificacionesAvanzadasCartonTest extends TestCase {

  /**
   * Verifica que los números del carton se encuentren en el rango 1 a 90.
   */
  public function testUnoANoventa() {
    $carton = new CartonEjemplo;
    foreach ($carton->filas() as $fila) {
      foreach (celdas_ocupadas($fila) as $celda) {
        $this->assertTrue(1 <= $celda && $celda <= 90);
      }
    }
  }

  /**
   * Verifica que cada fila de un carton tenga exactamente 5 celdas ocupadas.
   */
  public function testCincoNumerosPorFila() {
    $carton = new CartonEjemplo;
    foreach ($carton->filas() as $fila) {
      $this->assertTrue(count(celdas_ocupadas($fila)) == 5);
    }
  }

  /**
   * Verifica que para cada columna, haya al menos una celda ocupada.
   */
  public function testColumnaNoVacia() {
    $carton = new CartonEjemplo;
    foreach ($carton->columnas() as $columna) {
      $this->assertTrue(count(celdas_ocupadas($columna)) >= 1);
    }
  }

  /**
   * Verifica que no haya columnas de un carton con tres celdas ocupadas.
   */
  public function testColumnaCompleta() {
    $carton = new CartonEjemplo;
    foreach ($carton->columnas() as $columna) {
      $this->assertTrue(count(celdas_ocupadas($columna)) != 3);
    }
  }

  /**
   * Verifica que solo hay exactamente tres columnas que tienen solo una celda
   * ocupada.
   */
  public function testTresCeldasIndividuales() {
    $carton = new CartonEjemplo;
    $cantidadConUnaSolaOcupada = 0;
    foreach ($carton->columnas() as $columna) {
      if (count(celdas_ocupadas($columna)) == 1) {
        $cantidadConUnaSolaOcupada++;
      }
    }
    $this->assertTrue($cantidadConUnaSolaOcupada == 3);
  }

  /**
   * Verifica que los números de las columnas izquierdas son menores que los de
   * las columnas a la derecha.
   */
  public function testNumerosIncrementales() {
    $carton = new CartonEjemplo;
    $columnas = $carton->columnas();
    $mayores = [];
    $menores = [];

    foreach ($columnas as $columna) {
      $celdasDeLaColumna = array_values(celdas_ocupadas($columna));
      $menor = $celdasDeLaColumna[0];
      $mayor = $celdasDeLaColumna[0];
      foreach ($celdasDeLaColumna as $celda) {
        if ($menor > $celda) {
          $menor = $celda;
        }
        if ($mayor < $celda) {
          $mayor = $celda;
        }
      }
      $mayores[] = $mayor;
      $menores[] = $menor;
    }

    for ($i = 1; $i < count($columnas); ++$i) {
      $this->assertTrue($menores[$i] > $mayores[$i - 1]);
    }
  }

  /**
   * Verifica que en una fila no existan más de dos celdas vacias consecutivas.
   */
  public function testFilasConVaciosUniformes() {
    $carton = new CartonEjemplo;
    $filas = $carton->filas();
    foreach ($filas as $fila) {
      for ($i = 1; $i < count($fila); ++$i) {
        $this->assertFalse($fila[$i - 1] == 0 && $fila[$i] == 0);
      }
    }
  }

}
