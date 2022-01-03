#! /usr/bin/python

# 6ta Practica Laboratorio 
# Complementos Matematicos I
# Ejemplo parseo argumentos

import re
import sys
from random import random
from math import sqrt
import argparse
import matplotlib.pyplot as plt
import numpy as np

def vector_add(u, v):
    return (u[0] + v[0], u[1] + v[1])

def vector_difference(u, v):
    return (u[0] - v[0], u[1] - v[1])

def vector_length_squared(u):
    return u[0]**2 + u[1]**2

def vector_length(u):
    return sqrt(vector_length_squared(u))

def vector_scale(k, u):
    return (k * u[0], k * u[1])

def vector_normalize(u):
    return vector_scale(1 / vector_length(u), u)




def read_graph(filePath):
  """ Lee un grafo desde un archivo. """
  # Creamos un grafo vacío
  graph = ([], [])
  # Intentamos abrir el archivo
  try:
    with open(filePath) as fp:
      # Convertimos el archivo a una lista
      lines = []
      for line in fp:
        # Ignoramos espacios al principio y al final del renglón
        line = line.strip()
        # Ignoramos renglones vacíos
        if line != '':
          lines.append(line)
    # Chequeamos que el archivo no es vacío
    size = len(lines)
    if size == 0:
      print("Error: file is empty.")
      sys.exit()
    # Determinamos la cantidad de nodos en lo posible
    try:
      n = int(lines[0])
      # Chequeamos que hay suficientes nodos
      if n >= size:
        print("Error: not enough nodes.")
        sys.exit()
      # Chequamos que el número de nodos es válido
      if n < 0:
        print("Error: invalid number of nodes.")
        sys.exit()
      # Inicializamos las variables para leer los renglones
      invalid = False
      repeated = False
      i = 1
      # Leemos los vértices
      nodes = set()
      while i <= n and not invalid and not repeated:
        node = lines[i]
        # Chequeamos que el nombre del nodo no tenga espacios
        if re.search(r"\s", node):
          invalid = True
        # Chequeamos que no haya nodos repetidos
        elif node in nodes:
          repeated = True
        # Si es válido, agregamos el nodo
        else:
          graph[0].append(node)
          nodes.add(node)
        i += 1
      # Chequamos que los nodos son válidos
      if invalid:
        print("Error: invalid node.")
        sys.exit()
      # Chequeamos que todos los nodos son únicos
      if repeated:
        print("Error: repeated nodes.")
        sys.exit()
      # Leemos las aristas
      edges = set()
      while i < size and not invalid and not repeated:
        edge = tuple(lines[i].split())
        # Chequeamos que haya dos nodos válidos distintos
        if len(edge) != 2 or edge[0] == edge[1] or not edge[0] in nodes or not edge[1] in nodes:
          invalid = True
        # Chequeamos que no haya aristas repetidas
        elif edge in edges:
          print("ARISTA REPE")
          print(edge)
          repeated = True
        # Si es válido, agregamos la arista
        else:
          graph[1].append(edge)
          edges.add(edge)
          edges.add(edge[::-1])
        i += 1
      # Chequeamos que las aristas son válidas
      if invalid:
        print("Error: invalid edge.")
        sys.exit()
      # Chequamos que las aristas son únicas
      if repeated:
        print("Error: repeated edge.")
        sys.exit()
    # Si no se proporciona el número de nodos al inicio, terminamos la ejecución
    except ValueError:
      print("Error: number of nodes required.")
      sys.exit()
  # Si no se pudo abrir el archivo, terminamos la ejecución
  except OSError:
    print("Error: file cannot be opened.")
    sys.exit()
  # Retornamos el grafo
  return graph



class VerboseLogger:
    def __init__(self):
        pass

    def log(self, s):
        print(s)

    def notify_layout_started(self, vertex_count, edge_count):
        print("Performing layout on", vertex_count, "vertices, and", edge_count, "edges")

    def notify_layout_completed(self):
        print("Layout completed")

class QuietLogger:
    def __init__(self):
        pass

    def log(self, s):
        pass

    def notify_layout_started(self, vertex_count, edge_count):
        pass

    def notify_layout_completed(self):
        pass


class LayoutGraph:

    def __init__(self, grafo, iters, refresh, c1, c2, logger, width=400, height=400, padding=20, pause_time=0.1):
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

        # Guardo opciones
        self.iters = iters

        # TODO: faltan opciones
        self.refresh = refresh
        self.c1 = c1
        self.c2 = c2

        self.width = width
        self.height = height
        self.padding = padding
        self.pause_time = pause_time

        self.logger = logger

        # Inicializo estado
        # TODO
        self.positions = {}
        self.forces = {}

        self.randomize_positions()
        self.nulify_forces()


    def layout(self):
        """
        Aplica el algoritmo de Fruchtermann-Reingold para obtener (y mostrar)
        un layout
        """

        self.logger.notify_layout_started(self.number_of_vertices(), self.number_of_edges())

        for i in range(self.iters):
            self.step()
            self.draw()

        self.logger.notify_layout_completed()

        plt.show()


    def draw(self):
        plt.clf()
        plt.xlim(0 - self.padding, self.width + self.padding)
        plt.ylim(0 - self.padding, self.height + self.padding)

        xs = [self.positions[u][0] for u in self.vertices()]
        ys = [self.positions[u][1] for u in self.vertices()]

        plt.scatter(xs, ys, color="r")

        for (u, v) in self.edges():
            x1 = self.positions[u]
            x2 = self.positions[v]
            plt.plot([x1[0], x2[0]], [x1[1], x2[1]], color="b")

        plt.pause(self.pause_time)

    def step(self):
        self.nulify_forces()
        self.compute_attraction_forces()
        self.compute_repulsion_forces()
        self.compute_gravity_forces()
        self.update_positions()

    def compute_attraction_forces(self):
        for (u, v) in self.edges():
            force = self.compute_attraction_force(u, v)
            self.add_force(u, force)
            self.add_force(v, vector_scale(-1, force))

    def compute_repulsion_forces(self):
        for u in self.vertices():
            for v in self.vertices():
                if u is v:
                    continue
                force = self.compute_repulsion_force(u, v)
                self.add_force(u, force)

    def compute_gravity_forces(self):
        for u in self.vertices():
            force = self.compute_gravity_force(u)
            self.add_force(u, force)

    def update_positions(self):
        for u in self.vertices():
            self.positions[u] = vector_add(self.positions[u], self.forces[u])

    def compute_attraction_force(self, u, v):
        x1 = self.positions[u]
        x2 = self.positions[v]

        delta = vector_difference(x2, x1)

        d = vector_length(delta)

        direction = vector_normalize(delta)
        magnitude = self.attraction_function(self.k(), d)

        return vector_scale(magnitude, direction)

    def compute_repulsion_force(self, u, v):
        x1 = self.positions[u]
        x2 = self.positions[v]

        delta = vector_difference(x2, x1)

        d = vector_length(delta)

        direction = vector_normalize(delta)
        magnitude = self.repulsion_function(self.k(), d)

        return vector_scale(magnitude, direction)

    def compute_gravity_force(self, u):
        delta = vector_difference(self.centre(), self.positions[u])
        d = vector_length(delta)

        direction = vector_normalize(delta)
        magnitude = self.gravity_function(d)

        return vector_scale(magnitude, direction)

    def randomize_positions(self):
        for vertex in self.vertices():
            self.positions[vertex] = (random() * self.width, random() * self.height)

    def nulify_forces(self):
        for vertex in self.vertices():
            self.forces[vertex] = (0, 0)

    def attraction_function(self, k, d):
        return d**2 / (self.c2 * k)

    def repulsion_function(self, k, d):
        return -(self.c1 * k)**2 / d

    def gravity_function(self, d):
        return self.attraction_function(self.k(), d)

    def add_force(self, u, force):
        self.forces[u] = vector_add(self.forces[u], force)

    def k(self):
        return sqrt(self.area() / self.number_of_vertices())

    def number_of_vertices(self):
        return len(self.vertices())

    def number_of_edges(self):
        return len(self.edges())

    def vertices(self):
        return self.grafo[0]

    def edges(self):
        return self.grafo[1]

    def area(self):
        return self.width * self.height

    def centre(self):
        return (self.width / 2, self.height / 2)

def make_layout_graph(grafo, iters, refresh, c1, c2, width=400, height=400, padding=20, pause_time=0.1, verbose=False):
    logger = VerboseLogger() if verbose else QuietLogger()
    return LayoutGraph(grafo, iters, refresh, c1, c2, logger, width, height, padding, pause_time)

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
    # print(args.verbose)
    # print(args.iters)
    # print(args.file_name)
    # print(args.temp)
    # return

    # TODO: Borrar antes de la entrega
    grafo1 = ([1, 2, 3, 4, 5, 6, 7],
              [(1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 1)])
    
    grafo1 = graph_parser.read_graph(args.file_name)

    print("===========================")
    print(grafo1)
    print("===========================")

    # Creamos nuestro objeto LayoutGraph
    layout_gr = make_layout_graph(
        grafo1,  # TODO: Cambiar para usar grafo leido de archivo
        iters=args.iters,
        refresh=1,
        c1=0.1, # repulsion
        c2=30.0, # atraccion
        pause_time=0.1,
        verbose=args.verbose
    )
    
    # Ejecutamos el layout
    layout_gr.layout()
    return


if __name__ == '__main__':
    main()
