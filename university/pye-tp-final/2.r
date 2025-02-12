ensayo <- function(p) {
  x <- runif(1)
  return (x < p)
}

proceso <- function(p, k) {
  N <- 0
  res <- 0
  while (N != k) {
    if (ensayo(p)) {
      N <- N + 1
    }
    else {
      N <- 0
    }
    res <- res + 1
  }
  return(res)
}

esperanza <- function(p, k, n = 1000) {
  muestras <- sapply(1:n, function(x) { return (proceso(p, k))})
  return (list(muestras, mean(muestras)))
}

p <- 0.3
k <- 3

print(esperanza(p, k, n=10000))

N <- (p^(-k-1)-p^(-1))/(p^(-1)-1)
print(N)

N2 <- (p^(-k)-1)/(1-p)
print(N2)

# res <- esperanza(0.75, 10, n = 10000)
# muestras <- res[1][[1]]
# E <- res[2][[1]] 

