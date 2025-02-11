library("markovchain")

k = 100

crear_matriz <- function(k) {
  P = matrix(data=0, nrow=k+1, ncol=k+1)
  for (i in 0:k) {
    for (j in 0:k) {
      P[i+1, j+1] <- choose(k, j) * (i/k)^j * (1 - i/k)^(k-j)
    }
  }
  return(P)
}

P <- crear_matriz(k)
estados = sapply(0:k, as.character)
mc <- new("markovchain", states = estados, transitionMatrix = P)

# Ej a

steps <- 500
trayectoria <- rmarkovchain(mc, n=steps, t0="50")


plot(trayectoria, xlim=c(0,steps), ylim=c(0,k), type="l", xlab="generación", ylab="cantidad de genes tipo A")
print(trayectoria)

# Ej b

# P_00 y P_kk toman el valor 1.
# Esto se corresponde con la intuición del problema:
# - Si quedan 0 genes del alelo A en la población, este no se
#   puede reproducir, por lo que el numero se mantiene en 0.
# - Si toda la población está compuesto por genes del
#   alelo A, entonces el elelo a no se puede reproducir,
#   por lo que el numero de alelos A se mantiene en k.
print(P[0+1, 0+1])
print(P[k+1, k+1])

# Ej c

# TODO: conclusion

mean(markovchain::absorptionProbabilities(mc)[,1])
mean(markovchain::absorptionProbabilities(mc)[,2])
