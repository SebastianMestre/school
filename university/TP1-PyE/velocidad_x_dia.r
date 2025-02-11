install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

# boxplot de distancia/duracion por dia
# falta hacer los labels

recorridos$dia <- factor(recorridos$dia, levels = c(
  "Lunes",
  "Martes",
  "Miércoles",
  "Jueves",
  "Viernes",
  "Sábado",
  "Domingo"))

#boxplot(split(recorridos$distancia / recorridos$duracion, factor(recorridos$dia)))
boxplot(recorridos$distancia / recorridos$duracion ~ recorridos$dia,
        names = c("Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"),
        xlab = "Dia", ylab = "Velocidad Media [m/s]", main = "Velocidad Media por Dia")
