
# E_n: La señal es incorrecta en el momento n
sampleEn <- function(p, n) {
  en <- runif(1)
  if (en < p) {
    return(1)
  } else {
    return(0)
  }
}

# N_n: La cantidad de señales incorrectas hasta el momento n
sampleNn <- function(p, n) {
  nn <- 0
  for (i in 1:n) {
    nn <- nn + sampleEn(p, i)
  }
  return(nn)
}

valoresCaracteristicos <- function(muestras) {
  return(c(mean(muestras), var(muestras)))
}

analizarEn <- function(p, n, m=1000) {
  muestras <- sapply(1:m, function (x) { return(sampleEn(p,n)) })
  return(valoresCaracteristicos(muestras))
}

analizarNn <- function(p, n, m=1000) {
  muestras <- sapply(1:m, function (x) { return(sampleNn(p,n)) })
  return(valoresCaracteristicos(muestras))
}

# De acá en adelante, tomamos la probabilidad de que la señal
# sea incorrecta (y por tanto un ensallo sea exitoso) como p=0.37
p <- 0.37

# Simulamos una realizacion del proceso E_10
# En una ejecución de ejemplo dió 1
resultado <- sampleEn(p, 10)
print(resultado)

# Simulamos una realización del proceso N_10
# En una ejecución de ejemplo dió 4
resultado <- sampleNn(p, 10)
print(resultado)

# Simulamos 1000 realizaciones de cada proceso para estimar
# los valores característicos
m <- 1000

# En una ejecución de ejemplo estimamos
# E(E_n) = 0.3680000
# V(E_n) = 0.2328088
#
# Analiticamente, sabemos que los ensayos de bernoulli tienen los
# siguientes valores característicos:
# Esperanza = p       = 0.37
# Variancia = p - p^2 = 0.2331
resultado <- analizarEn(p, 10, m)
print(resultado)

# En una ejecución de ejemplo estimamos
# E(N_n) = 3.699000
# V(N_n) = 2.308708
#
# Analíticamente, sabemos que la suma de ensayos de bernoulli
# independientes sigue una distribución binomial, y que sus
# valores característicos son los siguientes:
# Esperanza = p*k           = 3.7
# Variancia = (p - p^2) * k = 2.331
# Donde k es la cantidad de ensayos (en este caso 10)
resultado <- analizarNn(p, 10, m)
print(resultado)