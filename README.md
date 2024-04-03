# Proxy Inverso y Balanceador de Carga para Aplicaciones HTTP

## Introducción
El objetivo de este proyecto es implementar un proxy inverso para HTTP y un balanceador de carga utilizando la API Sockets, además de esto la implementación de una aplicación cliente y de 3 servidores web. De modo que el cliente se conecta con el proxy, el proxy inverso intercepta las peticiones de los clientes y las reenvía a servidores capaces de procesarlas, mientras que el balanceador de carga distribuye las peticiones entrantes entre un conjunto de servidores, mejorando así la eficiencia y disponibilidad del sistema. Es importante aclarar que el balanceo de carga se hace por medio del algoritmo de Round Robin.

## Desarrollo 💻 🔧
### Servidor HTTP Proxy + Balanceador de Carga
El servidor se encarga de interceptar las peticiones de los clientes, reenviarlas a servidores destino seleccionados mediante el algoritmo de Round Robin, procesar las respuestas y retornarlas a los clientes. Se han implementado los siguientes aspectos:
* Escucha peticiones en el puerto 8080 y procesa solicitudes HTTP/1.1.
*	Implementa los métodos GET y HEAD.
*	Modifica las peticiones para enviarlas al servidor destino de forma adecuada.
*	Implementa un archivo de log para registrar todas las peticiones y respuestas.
*	Permite la caché de recursos solicitados por los clientes, almacenándolos en disco y estableciendo un Time To Live (TTL) configurable.
Se ejecuta de la siguiente forma: 
```
./servidor 8080 proxy_log.txt
```
### Aplicación Cliente HTTP
La aplicación cliente permite realizar peticiones HTTP a cualquier servidor HTTP, incluido el servidor HTTP Proxy + Balanceador de Carga. Se ejecuta de la siguiente forma: 
```
py cliente.py log.log url
```
url: url del recurso que deseamos solicitar
Permite registrar todas las peticiones realizadas en un archivo de log, que incluye información sobre la fecha, hora, tipo de solicitud HTTP y respuesta recibida del servidor/proxy. 
La aplicación cliente puede realizar peticiones utilizando los métodos GET, HEAD y POST.
Ofrece la funcionalidad de caché de recursos solicitados, con la capacidad de eliminar completamente el caché mediante el comando flush.
## Aspectos Logrados y No Logrados
### Aspectos Logrados : ✔️
*	Implementación funcional del servidor HTTP Proxy + Balanceador de Carga y la aplicación cliente. 
*	Cumplimiento de los requisitos especificados, incluyendo el procesamiento de solicitudes HTTP/1.1, la implementación de métodos GET y HEAD, la modificación de las peticiones para enviar el encabezado pedido, la caché de recursos, archivo log y mensajes con código de estado.
*	Implementación completa de la aplicación cliente con todos sus requerimientos, log, caché, uso de métodos de GET, HEAD e implementación de método POST, comando flush.

### Aspectos No Logrados : ❌

## Conclusiones
El proyecto ha permitido adquirir conocimientos sólidos en el diseño y desarrollo de aplicaciones concurrentes en red, así como en el manejo de peticiones HTTP y la implementación de servidores proxy y balanceadores de carga. Se han alcanzado los objetivos principales del proyecto, aunque quedan algunos aspectos fueron complicados de desarrollar puesto el impedimento que teníamos respecto al uso de algunas librerías.

##Referencias
*(https://www.youtube.com/watch?v=eshV2whJrqk)
*(https://www.youtube.com/watch?v=vF3TS4U6I_M)
*(https://www.youtube.com/watch?v=VEr-Gp86teY)
*(https://www.cloudflare.com/es-es/learning/dns/glossary/round-robin-dns/)

### Autores
* **Luis Miguel Giraldo Gonzalez**  - [SantiagoArias229](https://github.com/SantiagoArias229)
* **Vanessa Vélez Restrepo** - [vavelezr](https://github.com/vavelezr)
* **Santiago Arias Higuita** - [gotaluism](https://github.com/gotaluism)
