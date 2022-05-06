install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
cantusuarios <- length(usuarios$edad)

hist(usuarios$edad, main="Distrubucion de edad de los usuarios", col="#D5B7E8", xlab="Edad", ylab="Cant. de usuarios", xlim=range(10:70))


cortusuarios <- table(cut(usuarios$edad, breaks=10, dig.lab = 0))
cumusuarios <- cumsum(cortusuarios)
tablausuarios <- cbind(cortusuarios,cumusuarios,cortusuarios/cantusuarios,cumusuarios/cantususarios)
colnames(tablausuarios) <- (c("Frecuencia absoluta","Frecuencia absoluta acumulada","Frecuencia relativa","Frecuencia relativa acumulada"))
knitr::kable(tablausuarios)

recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

boxplot(usuarios$edad, main = "Distribucion de Usuarios por Edad")
usuarios_summ <- summary(usuarios$edad)
USUARIOS_MIN = usuarios_summ[1]
USUARIOS_MEDIAN = usuarios_summ[3]
USUARIOS_MEAN = usuarios_summ[4]
USUARIOS_MAX = usuarios_summ[5]

boxplot(usuariosrecorridos$edad, main = "Distribucion de Viajes por Edad")
hist(usuariosrecorridos$edad, xlab = "Edad", ylab = "Frecuencia", main = "Recorridos por Edad")
viajes_summ <- summary(usuarios$edad)
VIAJES_MEDIAN = viajes_summ[3]
VIAJES_MEAN = viajes_summ[4]

freq_table <- cbind("FRECUENCIA" = table(usuarios$edad),
      "FRECUENCIA RELATIVA"= table(usuarios$edad)/length(usuarios$edad), 
      "FRECUENCIA ACUMULADA" = cumsum(table(usuarios$edad)),
      "FRECUENCIA RELATIVA ACUMULADA" = cumsum(table(usuarios$edad)/length(usuarios$edad)))

totalFrecuenciaAbs <- dim(usuarios)[0]
totalFrecuenciaRel <- 1