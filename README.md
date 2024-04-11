# Proxy Inverso y Balanceador de Carga para Aplicaciones HTTP


# Tabla de Contenidos
1. [Introducción](#introducción)
2. [Flujo](#flujo)
3. [Desarrollo](#Desarrollo-)
   - [Servidor HTTP Proxy + Balanceador de Carga](#servidor-http-proxy--balanceador-de-carga)
   - [Aplicación Cliente HTTP](#aplicación-cliente-http)
   - [Servidores Web](#servidores-web)
4. [Aspectos Logrados y No Logrados](#aspectos-logrados-y-no-logrados)
   - [Aspectos Logrados](#aspectos-logrados-)
   - [Aspectos No Logrados](#aspectos-no-logrados-)
5. [Conclusiones](#conclusiones)
6. [Referencias](#referencias)
7. [Autores](#autores)


## Introducción
El objetivo de este proyecto es implementar un proxy inverso para HTTP y un balanceador de carga utilizando la API Sockets, además de esto la implementación de una aplicación cliente y de 3 servidores web. De modo que el cliente se conecta con el proxy, el proxy inverso intercepta las peticiones de los clientes y las reenvía a servidores capaces de procesarlas, mientras que el balanceador de carga distribuye las peticiones entrantes entre un conjunto de servidores, mejorando así la eficiencia y disponibilidad del sistema. Es importante aclarar que el balanceo de carga se hace por medio del algoritmo de Round Robin.

## Flujo
![Diagrama flujo telematica drawio](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/92fb3079-6249-4685-a67b-9ecd1448f9a2)

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
./servidor 
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

### Servidores Web
Los servidores web se realizaron con Apache, cada servidor tiene una pagina estatica en la cual hay un identificador con el cual se reconoce cual de los 3 servidores web es.
El servidor proxy es el que se encarga de redireccionar a uno de los 3 servidores web, no obstante si deseas acceder a uno de los servidores web por aparte, puedes copiar la siguiente dirección en el buscador de tu preferencia.
```
ipPublicaDeUnoDeLosServidoresWeb/test/testt.html
```
Ejemplo

![Captura de pantalla 2024-04-08 150519](https://github.com/gotaluism/ProyectoTelematica/assets/76192117/4c9c035a-8cf2-4296-b689-79f56a6c6e91)
Recuerda previamente haber prendido el servidor web en el aws 


## Aspectos Logrados y No Logrados
### Aspectos Logrados : ✔️
*	Implementación funcional del servidor HTTP Proxy + Balanceador de Carga y la aplicación cliente. 
*	Cumplimiento de los requisitos especificados, incluyendo el procesamiento de solicitudes HTTP/1.1, la implementación de métodos GET y HEAD y la modificación de las peticiones para enviar el encabezado pedido.
* Implementación del caché (verifica primero si la respuesta está en el caché y si está devuelve esa)
* Implementación del archivo log en donde se almacenan las solicitudes y las respuestas
* Mensajes con código de estado (error 500 en caso de solicitar un método no implementado)
* Uso del algoritmo Round Robin para el balanceo de carga
*	Implementación completa de la aplicación cliente con todos sus requerimientos, log, caché, uso de métodos de GET, HEAD e implementación de método POST, comando flush.



### Aspectos No Logrados : ❌

## Conclusiones
El proyecto ha permitido adquirir conocimientos sólidos en el diseño y desarrollo de aplicaciones concurrentes en red, así como en el manejo de peticiones HTTP y la implementación de servidores proxy y balanceadores de carga. Se han alcanzado los objetivos principales del proyecto, aunque quedan algunos aspectos fueron complicados de desarrollar puesto el impedimento que teníamos respecto al uso de algunas librerías.

## Referencias
* (https://www.youtube.com/watch?v=eshV2whJrqk)
* (https://www.youtube.com/watch?v=vF3TS4U6I_M)
* (https://www.youtube.com/watch?v=VEr-Gp86teY)
* (https://www.cloudflare.com/es-es/learning/dns/glossary/round-robin-dns/)

### Autores
* **Luis Miguel Giraldo Gonzalez**  - [gotaluism](https://github.com/gotaluism)
* **Vanessa Vélez Restrepo** - [vavelezr](https://github.com/vavelezr)
* **Santiago Arias Higuita** - [SantiagoArias229](https://github.com/SantiagoArias229)
