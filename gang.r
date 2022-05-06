install.packages("qcc") 
library(qcc)

usuarios <- read.table("usuarios8.csv", header = TRUE, sep = ",")
recorridos <- read.table("recorridos8.csv", header = TRUE, sep = ",")
usuariosrecorridos <- merge(usuarios, recorridos)

# estos pareto son medio malardos
pareto.chart(table(recorridos$direccion_estacion_origen), ylim = c(0, 25))
pareto.chart(table(recorridos$dia))

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
# ### edad --> medidas, boxplot, tabla de frecuencias por rango etario
#
# cuantitativo continuo
#
# ## recorrido
#
# ### direccion estacion origen --> cantidad de estaciones con n viajes --> bastones
#
# categorico
#
# ### direccion estacion destino --> cantidad de estaciones con n viajes --> bastones
#
# categorico
#
# ### duracion recorrido --> boxplot, medidas
#
# cuantitativo continuo
#
# ### distancia --> boxplot, medidas
#
# cuantitativo continuo
#
# ### dia --> bastones, tabla de frecuencias 
#
# # analisis bivariado
#
# ### edad x genero --> histograma
#
# ### dia x distancia/duracion --> boxplot