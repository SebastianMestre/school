install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")

# estos pareto son medio malardos
pareto.chart(table(recorridos$direccion_estacion_origen), ylim = c(0, 25))
pareto.chart(table(recorridos$dia))

usuariosrecorridos <- merge(usuarios, recorridos)

filtrarPorGenero <- function(usuarios, genero) {
  mascara <- apply(usuarios, MARGIN=c(1), FUN = function(x){ return(x[2] == genero) })
  filtrado <- usuarios[mascara, 1:3]
  return(filtrado)
}

# estos estan buenos para hacer histogramas
hombres <- filtrarPorGenero(usuarios, "M")
mujeres <- filtrarPorGenero(usuarios, "F")
nonbiny <- filtrarPorGenero(usuarios, "OTRO")

dotchart(recorridos$distancia, log = "x",)

scatter.smooth(recorridos$duracion_recorrido, recorridos$distancia, log = "xy")

scatter.smooth(recorridos$dia, recorridos$duracion_recorrido / recorridos$distancia, log="y")

# # variables
#
# ## usuario
#
# ### genero --> torta
#
# categorico
#
# ### edad --> medidas, boxplot
#
# cuantitativo continuo
#
# ## recorrido
#
# ### direccion estacion origen --> 
#
# categorico
#
# ### direccion estacion destino
#
# categorico
#
# ### duracion recorrido
#
# cuantitativo continuo
#
# ### distancia
#
# cuantitativo continuo
#
# ### dia --> histograma, 
#
#