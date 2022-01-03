from random import random
from math import sqrt
import matplotlib.pyplot as plt
import numpy as np
import vector_math as vmt

from notifications import VerboseLogger
from notifications import QuietLogger

# TODO: Temperatura
# TODO: Mas logging
# TODO: Limitar los vertices dentro del cuadro?

eps = 1

class LayoutGraph:

    def __init__(self, grafo, c1, c2, logger, iters, refresh, pause_time=0.1, width=400, height=400, padding=20):
        """
        Parámetros:
        grafo: grafo en formato lista
        c1: constante de repulsión
        c2: constante de atracción
        logger: objeto que notifica ciertos eventos importantes
        iters: cantidad de iteraciones a realizar
        refresh: cada cuántas iteraciones graficar. Si su valor es cero, entonces debe graficarse solo al final.
        pause_time: tiempo que se espera antes de graficar nuevamente.
        width: el ancho del cuadro dentro del cual se hace layout
        height: la altura del cuadro dentro del cual se hace layout
        padding: distancia minima del borde del cuadro en la que se posicionan los puntos inicialmente
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
            if self.refresh != 0 and i % self.refresh == 0:
                self.draw()
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
            self.add_force(v, vmt.scale(-1, force))

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
            self.positions[u] = vmt.add(self.positions[u], self.forces[u])

    def compute_attraction_force(self, u, v):
        x1 = self.positions[u]
        x2 = self.positions[v]

        delta = vmt.difference(x2, x1)

        d = vmt.length(delta)

        direction = vmt.normalize(delta)
        magnitude = self.attraction_function(self.k(), d)

        return vmt.scale(magnitude, direction)

    def compute_repulsion_force(self, u, v):
        x1 = self.positions[u]
        x2 = self.positions[v]

        delta = vmt.difference(x2, x1)

        d = vmt.length(delta)

        if d < eps:
            # TODO: alejar puntos en direccion aleatoria
            return (self.width/20, self.height/20)

        direction = vmt.normalize(delta)
        magnitude = self.repulsion_function(self.k(), d)

        return vmt.scale(magnitude, direction)

    def compute_gravity_force(self, u):
        delta = vmt.difference(self.centre(), self.positions[u])
        d = vmt.length(delta)

        direction = vmt.normalize(delta)
        magnitude = self.gravity_function(d)

        return vmt.scale(magnitude, direction)

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
        self.forces[u] = vmt.add(self.forces[u], force)

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
    """
    Parámetros:
    grafo: grafo en formato lista
    iters: cantidad de iteraciones a realizar
    refresh: cada cuántas iteraciones graficar. Si su valor es cero, entonces debe graficarse solo al final.
    c1: constante de repulsión
    c2: constante de atracción
    verbose: si está encendido, activa los comentarios
    """
    logger = VerboseLogger() if verbose else QuietLogger()
    return LayoutGraph(grafo, c1, c2, logger, iters, refresh, pause_time, width, height, padding)
