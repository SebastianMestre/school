import sys
import re

def read_graph(path):
    try:
        file_content = read_file(path)
    except OSError:
        show_error("No se pudo leer el archivo.")
    return parse_graph(file_content)

def parse_graph(file_content):
    lines = split_lines(file_content)

    if len(lines) == 0:
        show_error("El archivo no tiene contenido.")

    try:
        n = int(lines[0])
    except ValueError:
        show_error("La primera linea del archivo no contiene exactamente un entero.")

    if n < 0:
        show_error("La cantidad de nodos debe ser no negativa.")

    content_lines = lines[1:]

    if len(content_lines) < n:
        show_error("Se esperaban {} nodos, pero hay a lo sumo {}.".format(n, len(content_lines)))

    nodes = parse_nodes(content_lines[:n])
    edges = parse_edges(content_lines[n:], nodes)
    return (list(nodes), list(edges))

def parse_nodes(lines):
    nodes = set()
    for node in lines:
        error = validate_node(node, nodes)
        if error is not None:
            show_error(error)
        nodes.add(node)
    return nodes

def validate_node(node, nodes):
    if re.search(r"\s", node):
        return "Se encontro un nodo con un espacio: {}".format(node)

    if node in nodes:
        return "Se encontro un nodo repetido: {}".format(node)

    return None

def parse_edges(lines, nodes):
    edges = set()
    for line in lines:
        components = line.split()
        error = validate_edge(components, nodes, edges, line)
        if error is not None:
            show_error(error)
        edges.add(tuple(components))
    return edges

def validate_edge(components, nodes, edges, line):
    if len(components) != 2:
        return "Se encontro una arista con {} componente/s: {}".format(len(components), line)

    if components[0] not in nodes or components[1] not in nodes:
        return "Se encontro una arista que referencia nodos que no existen: {}".format(line)

    if components[0] == components[1]:
        return "Se encontro un lazo: {}".format(line)

    edge = tuple(components)

    if edge in edges or edge[::-1] in edges:
        return "Se encontro una arista repetida: {}".format(line)

    return None

def show_error(text):
    print("[ Error ] {}".format(text))
    sys.exit()

def split_lines(file_content):
    lines = file_content.splitlines()
    trimmed_lines = [line.strip() for line in lines]
    return [line for line in trimmed_lines if line != '']

def read_file(path):
    with open(path, "r") as fp:
        return fp.read()
