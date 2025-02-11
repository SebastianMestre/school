library(markovchain)

estados <- c("80", "135", "139", "445", "NO")
trans <- matrix(c(0,    0,    0,    0,    1,
                  0,    8/13, 3/13, 1/13, 1/13,
                  1/16, 3/16, 3/8,  1/4,  1/8,
                  0,    1/11, 4/11, 5/11, 1/11,
                  0,    1/8,  1/2,  1/8,  1/4), nrow = 5, byrow = TRUE)
pi_0 <- c(0, 0, 0, 0, 1)

mc <- new("markovchain", states = estados, transitionMatrix = trans, name = "mc7")

# a)

res <- pi_0 * mc ^ 7

# b)

# TODO