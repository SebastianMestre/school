<?php

namespace Ajedrez;

interface PiezaDeAjedrez {

  public function movimientosPosibles();

  public function posicionarEn(string $celda);

}
