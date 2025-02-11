<?php

namespace Ajedrez;

class Peon implements PiezaDeAjedrez {
    protected $color;
    protected $columna;
    protected $fila;

    public function __construct(string $posicion, $color){
        $this->color = $color;
        $this->columna = $posicion[0];
        $this->fila = $posicion[1];
    }

    public function movimientosPosibles(){
        $retorno = array();
        if($this->fila == '8')
        {
            return $retorno;
        }
        if($this->fila == '2')
        {
            $retorno[0] = (($this->columna).($this->fila+2));
            $retorno[1] = (($this->columna).($this->fila+1));
            return $retorno;
        }elseif($this->fila < '8')
        {
            $retorno[0] =(($this->columna).($this->fila+1));
            return $retorno;
        }
    }

    public function posicionarEn(string $posicion){
        $posPosibles = $this->movimientosPosibles();
        foreach($posPosibles as $i){
            if($i[0] == $posicion[0] && $i[1] == $posicion[1]){
                $this->columna= $posicion[0];
                $this->fila= $posicion[1];

            }
        }
    }
}

?>
