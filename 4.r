library(markovchain)

ruina <- function (S, p, name="mc") {
  estados <- mapply(as.character, 0:S)
  trans <- matrix(rep(0, (S+1)^2), (S + 1))
  trans[1,1] <- 1
  for (i in 1:(S-1)) {
    trans[i+1, i] <- 1 - p
    trans[i+1, i+2] <- p
  }
  trans[S+1, S+1] <- 1
  return(new("markovchain", states = estados, transitionMatrix = trans, name=name))
}

## a)

mc <- ruina(100, 0.5, "mc3")
# plot(rmarkovchain(mc,n=10,t0="7"))

## b)

se_arruina <- function(S, p, k) {
  mc <- ruina(S, p)
  estado <- as.character(k)
  while(estado != "0") {
    if (estado == as.character(S)) return(FALSE)
    estado = rmarkovchain(mc, n=1, t0=estado)
  }
  return(TRUE)
}

prob_ruina <- function(S, p, k, n = 100) {
  acc <- 0
  for (i in 1:n) {
    if (se_arruina(S, p, k)) {
      acc <- acc + 1 
    }
  }
  return (acc / n)
}

