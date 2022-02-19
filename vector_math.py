from math import sqrt, sin, cos

def add(u, v):
    return (u[0] + v[0], u[1] + v[1])

def difference(u, v):
    return (u[0] - v[0], u[1] - v[1])

def length_squared(u):
    return u[0]**2 + u[1]**2

def length(u):
    return sqrt(length_squared(u))

def scale(k, u):
    return (k * u[0], k * u[1])

def normalize(u):
    try:
        return scale(1 / length(u), u)
    except ZeroDivisionError:
        print("Error: trying to normalize zero vector.")
        raise

def angle2unit_vector(theta):
    return (cos(theta), sin(theta))

