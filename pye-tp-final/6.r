library(markovchain)

estados <- mapply(as.character, 1:5)
trans <- matrix(c(0,   1/2, 1/2, 0,   0,
                  1/5, 1/5, 1/5, 1/5, 1/5,
                  1/3, 1/3, 0,   1/3, 0,
                  0,   0,   0,   0,   1,
                  0,   0,   1/2, 1/2, 0), nrow = 5, byrow = TRUE)
mc <- new("markovchain", states = estados, transitionMatrix = trans, name = "mc6")

# a)
plot(mc)

# b)
n <- 100
probs <- c(1,0,0,0,0) * mc ^ 100