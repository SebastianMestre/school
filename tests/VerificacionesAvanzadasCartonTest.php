<?php

namespace Bingo;

use PHPUnit\Framework\TestCase;

class VerificacionesAvanzadasCartonTest extends TestCase {

  public static function cartonProvider () {
    return [
      [new CartonEjemplo],
      [new CartonJs],
      [new Carton((new FabricaCartones)->generarCarton())]
    ];
  }
  
  /**
   * Verifica que los números del carton se encuentren en el rango 1 a 90.
   *
   * @dataProvider cartonProvider
   */
  public function testUnoANoventa(CartonInterface $carton) {
    foreach ($carton->filas() as $fila) {
      foreach (celdas_ocupadas($fila) as $celda) {
        $this->assertTrue(1 <= $celda && $celda <= 90);
      }
    }
  }
  


  /**
   * Verifica que cada fila de un carton tenga exactamente 5 celdas ocupadas.
   *
   * @dataProvider cartonProvider
   */
  public function testCincoNumerosPorFila(CartonInterface $carton) {
    foreach ($carton->filas() as $fila) {
      $this->assertTrue(count(celdas_ocupadas($fila)) == 5);
    }
  }

  /**
   * Verifica que para cada columna, haya al menos una celda ocupada.
   *
   * @dataProvider cartonProvider
   */
  public function testColumnaNoVacia(CartonInterface $carton) {
    foreach ($carton->columnas() as $columna) {
      $this->assertTrue(count(celdas_ocupadas($columna)) >= 1);
    }
  }

  /**
   * Verifica que no haya columnas de un carton con tres celdas ocupadas.
   *
   * @dataProvider cartonProvider
   */
  public function testColumnaCompleta(CartonInterface $carton) {
    foreach ($carton->columnas() as $columna) {
      $this->assertTrue(count(celdas_ocupadas($columna)) != 3);
    }
  }

  /**
   * Verifica que solo hay exactamente tres columnas que tienen solo una celda
   * ocupada.
   *
   * @dataProvider cartonProvider
   */
  public function testTresCeldasIndividuales(CartonInterface $carton) {
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
   *
   * @dataProvider cartonProvider
   */
  public function testNumerosIncrementales(CartonInterface $carton) {
    $columnas = $carton->columnas();
    
    $mayores = [];
    $menores = [];
    foreach ($columnas as $columna) {
      $celdasDeLaColumna = celdas_ocupadas($columna);
      $mayores[] = max($celdasDeLaColumna);
      $menores[] = min($celdasDeLaColumna);
    }

    for ($i = 1; $i < count($columnas); ++$i) {
      $this->assertTrue($menores[$i] > $mayores[$i - 1]);
    }
  }

  /**
   * Verifica que en una fila no existan más de dos celdas vacias consecutivas.
   *
   * @dataProvider cartonProvider
   */
  public function testFilasConVaciosUniformes(CartonInterface $carton) {
    foreach ($carton->filas() as $fila) {
      for ($i = 2; $i < count($fila); ++$i) {
        $this->assertFalse($fila[$i - 2] == 0 && $fila[$i - 1] == 0 && $fila[$i] == 0);
      }
    }
  }

}
