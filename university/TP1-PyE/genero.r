install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)


# Genero
filtrarPorGenero <- function(usuarios, genero) {
  mascara <- apply(usuarios, MARGIN=c(1), FUN = function(x){ return(x[2] == genero) })
  filtrado <- usuarios[mascara, 1:3]
  return(filtrado)
}

## Univariado

# estos estan buenos para hacer histogramas
hombres <- filtrarPorGenero(usuarios, "M")
#hist(hombres$edad)
#hist(hombres$edad_usuario, breaks = 5*0:10 + 15, main = "Hombres por edad", xlab = "edad", ylab = "frecuencia")
mujeres <- filtrarPorGenero(usuarios, "F")
nonbiny <- filtrarPorGenero(usuarios, "OTRO")

viajes_hombres = filtrarPorGenero(usuariosrecorridos, "M")
viajes_mujeres = filtrarPorGenero(usuariosrecorridos, "F")
viajes_nonbiny = filtrarPorGenero(usuariosrecorridos, "OTRO")

pieUsuariosXGenero <- function() {
  cant_usuarios_genero = c(dim(hombres)[1], dim(mujeres)[1], dim(nonbiny)[1])
  labels_usuarios_genero = c(paste("Hombres ", dim(hombres)[1]/dim(usuarios)[1]*100, "%", sep = ""),
                             paste("Mujeres ", dim(mujeres)[1]/dim(usuarios)[1]*100, "%", sep = ""),
                             paste("Otro ", dim(nonbiny)[1]/dim(usuarios)[1]*100, "%", sep = ""))
  pie(cant_usuarios_genero,
      labels = labels_usuarios_genero,
      col=c("#82B5F1","#E84B89","#E5EC6D"),
      main = "Usuarios por Genero")
}

pieViajesXGenero <- function() {
  cant_viajes_genero = c(dim(viajes_hombres)[1], dim(viajes_mujeres)[1], dim(viajes_nonbiny)[1])
  labels_viajes_genero = c(paste("Hombres ", round(dim(viajes_hombres)[1]/dim(usuariosrecorridos)[1]*100), "%", sep = ""),
                           paste("Mujeres ", round(dim(viajes_mujeres)[1]/dim(usuariosrecorridos)[1]*100), "%", sep = ""),
                           paste("Otros ", round(dim(viajes_nonbiny)[1]/dim(usuariosrecorridos)[1]*100), "%", sep = ""))
  pie(cant_viajes_genero,
      labels = labels_viajes_genero,
      col=c("#82B5F1","#E84B89","#E5EC6D"),
      main = "Viajes por Genero") 
}


## Bivariado

# ajustar colores
# referencia de colores?? 
plotEdadXGenero <- function() {
  hm <- hist(mujeres$edad_usuario, plot = FALSE)
  hh <- hist(hombres$edad_usuario, plot = FALSE)
  hnb <- hist(nonbiny$edad_usuario, plot = FALSE)
  plot(hm, col = "#E84B89",
       main = "Edad por Genero",
       xlab = "Edad",
       ylab = "Frecuencia")
  plot(hh, col = "#82B5F1", add = TRUE)
  plot(hnb, col = "#E5EC6D", add = TRUE)
}

tablita <- table(usuarios$genero_usuario,usuarios$edad_usuario)

tt <- rbind(cumsum(tablita[1,]),cumsum(tablita[2,]),cumsum(tablita[3,]))

boxplot(usuarios$edad_usuario ~ usuarios$genero_usuario,
        col=c("#82B5F1","#E84B89","#E5EC6D"),
        xlab = "Género del usuario", 
        ylab = "Edad del usuario [años]",
        main = "Distribución de la edad por género del usuario ")