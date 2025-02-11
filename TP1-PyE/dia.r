install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

recorridos$dia <- factor(recorridos$dia, levels = c(
  "Lunes",
  "Martes",
  "MiÃ©rcoles",
  "Jueves",
  "Viernes",
  "SÃ¡bado",
  "Domingo"))

freq <- table(recorridos$dia)
barplot(freq, names.arg = c(
  "Lunes",
  "Martes",
  "Miercoles",
  "Jueves",
  "Viernes",
  "Sabado",
  "Domingo"), 
  xlab = "Dia", ylab = "Frecuencia", main = "Viajes por Dia")

freqTable = cbind("Frecuencia Abs" = freq,
                  "Frecuencia Rel" = freq / length(recorridos$dia),
                  "Frecencia Acum Abs" = cumsum(freq),
                  "Frecuencia Acum Rel" = cumsum(freq / length(recorridos$dia)))
row.names(freqTable) <- c(
  "Lunes",
  "Martes",
  "Miercoles",
  "Jueves",
  "Viernes",
  "Sabado",
  "Domingo") 

totalFreqAbs <- length(recorridos$dia)
totalFreqRel <- 1
