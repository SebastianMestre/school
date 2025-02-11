# Helper para tests

# Archivo -> String
# Toma un archivo y retorna el contenido
def get_content(path):
    f = open(path, 'r')
    content = f.read()
    f.close()
    return content