poisson <- function(l, dt) {
  p <- l*dt*exp(-l*dt)
  x <- runif(1)
  return (x < p)
}

lambda <- 0.1
kms <- 10000
dt <- 0.1
n <- kms / dt

N <- NULL
D <- NULL
N_t <- 0
D_t <- 0
for (t in 1:n) {
  if (poisson(lambda, dt)) {
    N_t <- N_t + 1
    N <- rbind(N, c(t*dt, N_t))
    D <- rbind(D, c(N_t, D_t))
    D_t <- 0
  }
  D_t <- D_t + dt
}
plot(N, type = "s")
hist(N[,1])
hist(D, breaks=100)
