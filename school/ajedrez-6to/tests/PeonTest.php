<?php

namespace Ajedrez;

use PHPUnit\Framework\TestCase;

class PeonTest extends TestCase {

  public function testAvancePeon() {
    $columnas = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'];

    foreach ($columnas as $columna) {
      $peon = new Peon($columna . '2', 'Blanco');

      $movimientos = $peon->movimientosPosibles();
      $esperando = [$columna . '4', $columna . '3'];

      $this->assertEquals(
        $movimientos,
        $esperando
      );
    }
  }

}
