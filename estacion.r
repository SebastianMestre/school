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
        col = c("#77DDEF", "#FC926E"),
        labels = c(paste("Misma estacion ", round(dim(recorridos[mismaEstacion,])[1] / dim(recorridos)[1] * 100), "%", sep = ""), paste("Diferente estacion ", round(dim(recorridos[!mismaEstacion,])[1] / dim(recorridos)[1] * 100), "%", sep = "")))


### plot(vector) hace un grafico de bastones ;)

freq_estacion_origen <- table(recorridos$direccion_estacion_origen) 
freq_viajes_origen <- table(freq_estacion_origen)
plot(freq_viajes_origen, 
     xlab = "Cantidad de viajes", 
     ylab = "Cantidad de estaciones",
     main = "Cantidad de estaciones origen de x viajes")

freq_estacion_destino <- table(recorridos$direccion_estacion_destino) 
freq_viajes_destino <- table(freq_estacion_destino)
### Tal vez estaria bueno limitar x para q no se vean los outliers
plot(freq_viajes_destino, 
     xlab = "Cantidad de viajes", 
     ylab = "Cantidad de estaciones",
     main = "Cantidad de estaciones destino de x viajes")

