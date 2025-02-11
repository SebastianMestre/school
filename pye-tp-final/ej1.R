
sampleXi <- function() {
  xi <- runif(1)
  return(xi)
}

sampleE <- function() {
  e <- 0
  for (i in 1: 40) {
    e <- e + sampleXi()
  }
  return(e)
}

n <- 10000
muestras <- sapply(1:n, function(x) { return(sampleE()) })

print(mean(muestras))
print(var(muestras))
hist(muestras, 50, xlim=c(10,30), probability = TRUE, ylim=c(0, 0.3))
