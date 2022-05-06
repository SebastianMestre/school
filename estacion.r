install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

# Grafico de barras: recorridos con origen = destino vs el resto
mismaEstacion <- recorridos$direccion_estacion_origen == recorridos$direccion_estacion_destino
barplot(c(dim(recorridos[mismaEstacion,])[1] / dim(recorridos)[1],
        dim(recorridos[!mismaEstacion,])[1]/dim(recorridos)[1]),
        col = c("blue", "red"),
        names.arg = c("Misma estacion", "Diferente estacion"))

# Lo mismo pero pie
# falta agregar porcentajes
pie(c(dim(recorridos[mismaEstacion,])[1] / dim(recorridos)[1],
          dim(recorridos[!mismaEstacion,])[1]/dim(recorridos)[1]),
        col = c("blue", "red"),
        labels = c("Misma estacion", "Diferente estacion"))

##
### Faltaria hacer grafico de bastones (cant de viajes, cant de estaciones)
### no tengo ni idea cual seria la forma mas facil
### plot(vector) hace un grafico de bastones ;)
##