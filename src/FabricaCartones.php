<?php

namespace Bingo;

class FabricaCartones {

  public function generarCarton() {
    // Algo de pseudo-código para ayudar con la evaluacion.
      $carton = $this->intentoCarton();
      while(!($this->cartonEsValido($carton))){
        $carton = $this->intentoCarton();
      }
      return $carton;    
  }

  protected function cartonEsValido($carton) {
    if ($this->validarUnoANoventa($carton) &&
      $this->validarCincoNumerosPorFila($carton) &&
      $this->validarColumnaNoVacia($carton) &&
      $this->validarColumnaCompleta($carton) &&
      $this->validarTresCeldasIndividuales($carton) &&
      $this->validarNumerosIncrementales($carton) &&
      $this->validarFilasConVaciosUniformes($carton)
    ) {
      return TRUE;
    }
    return FALSE;
  }

  protected function validarUnoANoventa($carton) {
    foreach ($carton as $fila) {
      foreach (array_filter($fila) as $celda) {
        if (1 <= $celda && $celda <= 90) {
        } else { return false; }
      }
    }
    return true;
  }

  protected function validarCincoNumerosPorFila($carton) {
    foreach ($columnas as $indice_columna => $fila) {
      foreach ($fila as $indice_fila => $celda) {
        $filas[$indice_fila][$indice_columna] = $celda;
      }
    }
    foreach ($filas as $fila) {
      if(count(celdas_ocupadas($fila)) != 5)
        return false;
    }
    return true;
  }

  protected function validarColumnaNoVacia($carton) {
    foreach ($carton as $columna) {
      if(count(array_filter($columna)) < 1)
        return false;
    }
    return true;
  }

  protected function validarColumnaCompleta($carton) {
    foreach ($carton as $columna) {
      if(count(celdas_ocupadas($columna)) == 3)
        return false;
    }
    return true;
  }

  protected function validarTresCeldasIndividuales($carton) {
    $cantidadConUnaSolaOcupada = 0;
    foreach ($carton as $columna) {
      if (count(celdas_ocupadas($columna)) == 1) {
        $cantidadConUnaSolaOcupada++;
      }
    }
    return ($cantidadConUnaSolaOcupada == 3);
  }

  protected function validarNumerosIncrementales($carton) {
    $columnas = $carton;
    
    $mayores = [];
    $menores = [];
    foreach ($columnas as $columna) {
      $celdasDeLaColumna = celdas_ocupadas($columna);
      $mayores[] = max($celdasDeLaColumna);
      $menores[] = min($celdasDeLaColumna);
    }

    for ($i = 1; $i < count($columnas); ++$i) {
      if($menores[$i] <= $mayores[$i - 1])
        return false;
    }
    return true;
  }

  protected function validarFilasConVaciosUniformes($carton) {
    foreach ($columnas as $indice_columna => $fila) {
      foreach ($fila as $indice_fila => $celda) {
        $filas[$indice_fila][$indice_columna] = $celda;
      }
    }
    foreach ($filas as $fila) {
      for ($i = 2; $i < count($fila); ++$i) {
        if($fila[$i - 2] == 0 && $fila[$i - 1] == 0 && $fila[$i] == 0)
          return false;
      }
    }
    return true;
  }


  public function intentoCarton() {
    $contador = 0;

    $carton = [
      [0,0,0],
      [0,0,0],
      [0,0,0],
      [0,0,0],
      [0,0,0],
      [0,0,0],
      [0,0,0],
      [0,0,0],
      [0,0,0]
    ];
    $numerosCarton = 0;

    while ($numerosCarton < 15) {
      $contador++;
      if ($contador == 50) {
        return $this->intentoCarton();
      }
      $numero = rand (1, 90);

      $columna = floor ($numero / 10);
      if ($columna == 9) { $columna = 8;}
      $huecos = 0;
      for ($i = 0; $i<3; $i++) {
        if ($carton[$columna][$i] == 0) {
          $huecos++;
        }
        if ($carton[$columna][$i] == $numero) {
          $huecos = 0;
          break;
        }
      }
      if ($huecos < 2) {
        continue;
      }

      $fila = 0;
      for ($j=0; $j<3; $j++) {
        $huecos = 0;
        for ($i = 0; $i<9; $i++) {
          if ($carton[$i][$fila] == 0) { $huecos++; }
        }

        if ($huecos < 5 || $carton[$columna][$fila] != 0) {
          $fila++;
        } else{
          break;
        }
      }
      if ($fila == 3) {
        continue;
      }

      $carton[$columna][$fila] = $numero;
      $numerosCarton++;
      $contador = 0;
    }

    for ( $x = 0; $x < 9; $x++) {
      $huecos = 0;
      for ($y =0; $y < 3; $y ++) {
        if ($carton[$x][$y] == 0) { $huecos++;}
      }
      if ($huecos == 3) {
        return $this->intentoCarton();
      }
    }

    return $carton;
  }


}
