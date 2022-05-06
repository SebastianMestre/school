install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

boxplot(recorridos$duracion / 60,
        outline = FALSE,
        horizontal = TRUE,
        xlab = "Tiempo [minutos]",
        main = "Duracion Recorridos (sin outliers)",
        col = c("red"))

summ <- round(summary(recorridos$duracion / 60))
MIN <- summ[1]
MEDIAN <- summ[3]
MEAN <- summ[4]
MAX <- summ[6]