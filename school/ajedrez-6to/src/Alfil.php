<?php

namespace Ajedrez;

class Alfil implements PiezaDeAjedrez {
	protected $columna;
	protected $fila;
	protected $color;

	public function __construct(string $celda, $color){
		$this->posicionarEn($celda);
		$this->color = $color;
	}

	public function movimientosPosibles(){
                $listaMovimientosPosibles = array();
                $listaMovimientosPosibles = array_merge(
                        $this->listaMovimientosEnDireccion(1, 1),       // Arriba Derecha
                        $this->listaMovimientosEnDireccion(1, -1),      // Arriba Izquierda
                        $this->listaMovimientosEnDireccion(-1, 1),      // Abajo Derecha
                        $this->listaMovimientosEnDireccion(-1, -1)      // Abajo Izquierda
                );
                return $listaMovimientosPosibles;
  	}

        public function posicionarEn(string $celda){
                $this->columna = ord($celda[0]);
		$this->fila = $celda[1];
	}

        protected function listaMovimientosEnDireccion(int $modFila, int $modColumna){
                $movimientos = array();
                $auxColumna = $this->columna;
                $auxFila = $this->fila;

                $auxColumna += $modColumna;
                $auxFila += $modFila;
                while($auxColumna > 64 && $auxColumna < 73 && $auxFila < 9 && $auxFila > 0){
                        array_push($movimientos, chr($auxColumna) . $auxFila);
                        $auxColumna += $modColumna;
                        $auxFila += $modFila;
                }

                return $movimientos;
        }
}
