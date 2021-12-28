#! /usr/bin/python

# 6ta Practica Laboratorio 
# Complementos Matematicos I
# Ejemplo parseo argumentos

import argparse
import matplotlib.pyplot as plt
import numpy as np


class LayoutGraph:

    def __init__(self, grafo, iters, refresh, c1, c2, verbose=False):
        """
        Parámetros:
        grafo: grafo en formato lista
        iters: cantidad de iteraciones a realizar
        refresh: cada cuántas iteraciones graficar. Si su valor es cero, entonces debe graficarse solo al final.
        c1: constante de repulsión
        c2: constante de atracción
        verbose: si está encendido, activa los comentarios
        """

        # Guardo el grafo
        self.grafo = grafo

        # Inicializo estado
        # TODO
        self.positions = {}
        self.forces = {}

        # Guardo opciones
        self.iters = iters
        self.verbose = verbose
        # TODO: faltan opciones
        self.refresh = refresh
        self.c1 = c1
        self.c2 = c2

    def layout(self):
        """
        Aplica el algoritmo de Fruchtermann-Reingold para obtener (y mostrar)
        un layout
        """

        for i in range(self.iters):
            draw()
            step()

    def draw(self):
        # TODO
        pass

    def step(self):
        self.initialize_accumulators()
        self.compute_attraction_forces()
        self.compute_repulsion_forces()
        self.compute_gravity_forces()
        self.update_positions()

    def initialize_accumulators(self):
        # TODO
        pass

    def compute_attraction_forces(self):
        # TODO
        pass

    def compute_repulsion_forces(self):
        k = self.k()
        for u in self.vertices():
            for v in self.vertices():
                if u is v:
                    continue
                force = compute_repulsion_force(u, v)
                self.add_force(u, force)

    def compute_gravity_forces(self):
        # TODO
        pass

    def update_positions(self):
        # TODO
        pass

    def compute_repulsion_force(self, u, v):
        x1 = self.positions[u]
        x2 = self.positions[v]

        delta = vector_difference(x2, x1)

        d = vector_length(delta)

        direction = vector_normalize(delta)
        magnitude = self.repulsion_function(k, d)

        return vector_scale(magnitude, direction)

    def attraction_function(self, k, d):
        return d**2 / (self.c2 * k)

    def repulsion_function(self, k, d):
        return -(self.c1 * k)**2 / d

    def add_force(self, u, force):
        self.forces[u] = vector_add(self.forces[u], fuerza)

    def vertices(self):
        return self.grafo[0]

    def edges(self):
        return self.grafo[1]

    def k(self):
        return sqrt(self.area() / self.number_of_vertices())

    def number_of_vertices(self):
        return len(self.vertices())

def make_layout_graph(graph, iters, refresh, c1, c2, verbose=False):
    return LayoutGraph(graph, iters, refresh, c1, c2, verbose)

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
    print(args.verbose)
    print(args.iters)
    print(args.file_name)
    print(args.temp)
    # return

    # TODO: Borrar antes de la entrega
    grafo1 = ([1, 2, 3, 4, 5, 6, 7],
              [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 1)])
    
    # Creamos nuestro objeto LayoutGraph
    layout_gr = make_layout_graph(
        grafo1,  # TODO: Cambiar para usar grafo leido de archivo
        iters=args.iters,
        refresh=1,
        c1=0.1,
        c2=5.0,
        verbose=args.verbose
    )
    
    # Ejecutamos el layout
    layout_gr.layout()
    return


if __name__ == '__main__':
    main()
