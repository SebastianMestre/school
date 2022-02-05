#! /usr/bin/python

# 6ta Practica Laboratorio 
# Complementos Matematicos I
# Ejemplo parseo argumentos

import sys
import argparse
import parser as graph_parser

from layout import make_layout_graph

def main():
    # Definimos los argumentos de linea de comando que aceptamos
    parser = argparse.ArgumentParser()

    # Verbosidad, opcional, False por defecto
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Muestra mas informacion al correr el programa'
    )
    # Cantidad de iteraciones, opcional, 50 por defecto
    parser.add_argument(
        '--iters',
        type=int,
        help='Cantidad de iteraciones a efectuar',
        default=50
    )
    # Refresh, opcional, 1 por defecto.
    parser.add_argument(
        '--refresh',
        type=int,
        help='',
        default=1
    ) 
    # Temperatura inicial
    parser.add_argument(
        '--temp',
        type=float,
        help='Temperatura inicial',
        default=100.0
    )
    # Archivo del cual leer el grafo
    parser.add_argument(
        'file_name',
        help='Archivo del cual leer el grafo a dibujar'
    )

    args = parser.parse_args()

    # Descomentar abajo para ver funcionamiento de argparse
    # print(args.verbose)
    # print(args.iters)
    # print(args.file_name)
    # print(args.temp)
    # return

    grafo1 = graph_parser.read_graph(args.file_name)

    print("===========================")
    print(grafo1)
    print("===========================")

    # Creamos nuestro objeto LayoutGraph
    layout_gr = make_layout_graph(
        grafo1,
        iters=args.iters,
        refresh=args.refresh,
        c1=0.1, # repulsion
        c2=30.0, # atraccion
        temperature=args.temp,
        pause_time=0.01,
        verbose=args.verbose
    )
    
    # Ejecutamos el layout
    layout_gr.layout()
    return


if __name__ == '__main__':
    main()
