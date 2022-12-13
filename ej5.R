library("markovchain")

k = 50

P = matrix(rep(0, (k+1)*(k+1)), nrow=k+1)
for (i in 0:k) {
  for (j in 0:k) {
    P[i+1, j+1] <- choose(k, j) * (i/k)^j * (1 - i/k)^(k-j)
  }
}

stateNames = sapply(0:k, function(x) { as.character(x) })
mc <- new("markovchain", states = stateNames, transitionMatrix = P)

# Ej a
steps <- 100
result <- rmarkovchain(mc, n=steps, t0="25")
plot(result, xlim=c(0,steps), ylim=c(0,k))
print(result)

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

mean(markovchain::absorptionProbabilities(mc)[,1])
mean(markovchain::absorptionProbabilities(mc)[,2])