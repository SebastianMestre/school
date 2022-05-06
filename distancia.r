install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

boxplot(recorridos$distancia / 1000,
        outline = FALSE,
        horizontal = TRUE,
        xlab = "Distancia [km]",
        main = "Distancia Recorridos (sin outliers)",
        col = c("green"))

summ <- round(summary(recorridos$distancia))
MIN <- summ[1]
MEDIAN <- summ[3]
MEAN <- summ[4]
MAX <- summ[6]