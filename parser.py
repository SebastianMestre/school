
import re

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
