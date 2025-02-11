<?php

namespace Ajedrez;

class Rey implements PiezaDeAjedrez {

	protected $columna = "D";
	protected $fila = 1;
  	protected $color = "Blanco";

	public function movimientosPosibles()
	{
		$movimientos = [];

		if (1<$this->fila<9)
		{$movimientos = $this->columna . strval((this->fila)-1);}

		if (0<$this->fila<7)
		{$movimientos = $this->columna . strval((this->fila)+1);}

		if (ord("A")<ord($this->columna)<ord("I"))
		{$movimientos = chr((ord($this->columna))-1) . strval(this->fila);}

		if (ord("A")<=ord($this->columna)<ord("G"))
		{$movimientos = chr((ord($this->columna))+1) . strval(this->fila);}

		if (0<$this->fila<7 && ord("A")<=ord($this->columna)<ord("G"))
		{$movimientos =chr((ord($this->columna))+1) . strval((this->fila)+1);}

		if (1<$this->fila<9 && ord("A")<ord($this->columna)<ord("I"))
		{$movimientos =chr((ord($this->columna))-1) . strval((this->fila)-1);}

		if (0<$this->fila<7 && ord("A")<ord($this->columna)<ord("I"))
		{$movimientos =chr((ord($this->columna))+1) . strval((this->fila)-1);}

		if (1<$this->fila<9 && ord("A")<=ord($this->columna)<ord("G"))
		{$movimientos =chr((ord($this->columna))-1) . strval((this->fila)+1);}

		return $movimientos; //Devuelve una lista de celdas posibles
	}

	 public function posicionarEn(string $celda)
	 {
                $this->columna = ord($celda[0]);
		$this->fila = $celda[1];
	}

}
