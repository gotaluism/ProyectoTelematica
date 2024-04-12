<h1 align="center">ST0255 TELEMÁTICA</h1>
<h1 align="center">PROYECTO #1</h1>
<h1 align="center">TEMÁTICA: PROGRAMACIÓN EN RED A PARTIR DE LA CREACIÓN DE UN PROXY INVERSO + BALANCEADOR DE CARGA Y CLIENTE</h1>

## Video explicativo
Link del video explicativo: https://youtu.be/7tvadK4IWd4


## Tabla de Contenidos
- [Introducción](#introducción)
- [Prerequisitos](#prerquisitos)
- [Desarrollo](#desarrollo)
  - [Servidor HTTP Proxy + Balanceador de Carga](#servidor-http-proxy--balanceador-de-carga)
  - [Aplicación Cliente HTTP](#aplicación-cliente-http)
  - [Servidores Web](#servidores-web)
- [Diagrama de flujo del proyecto ](#diagrama-de-flujo-del-proyecto)
- [Aspectos Logrados y No Logrados](#aspectos-logrados-y-no-logrados)
- [Conclusiones](#conclusiones)
- [Referencias](#referencias)
- [Autores](#autores)


## Introducción

Este proyecto se enmarca dentro de la asignatura de Telemática de la Universidad EAFIT. El propósito central del proyecto es desarrollar un sistema robusto que incluye un proxy inverso para HTTP y un balanceador de carga, aprovechando la API de Sockets para la creación de un protocolo de comunicación efectivo entre una aplicación cliente y múltiples servidores web.

La implementación se centra en establecer un proxy inverso que actúa como intermediario entre el cliente y los servidores, interceptando todas las peticiones HTTP del cliente. Este proxy no solo redirige estas peticiones a los servidores que están en condiciones de procesarlas sino que también gestiona la respuesta hacia el cliente, optimizando el proceso mediante un algoritmo de balanceo de carga de Round Robin. Este método de distribución de carga se encarga de asignar las peticiones entrantes a diferentes servidores de forma rotativa y equitativa, lo que mejora significativamente la eficiencia y la disponibilidad del sistema.

El escenario implementado contempla tres servidores web que funcionan en conjunto con el proxy y el balanceador para manejar las peticiones de manera distribuida usando round robin. Cada servidor aloja una copia idéntica de una página web, lo cual no solo simula un entorno de producción realista sino que también permite evaluar efectivamente la distribución de carga entre los diferentes nodos.

Además, se desarrollará una aplicación cliente que permitirá realizar peticiones HTTP a cualquier servidor HTTP, incluido el servidor HTTP Proxy + Balanceador de Carga. Esta aplicación estará diseñada para enviar peticiones sin el uso de librerías especializadas, utilizando únicamente la funcionalidad proporcionada por la API de Sockets. La aplicación cliente se encargará de armar las peticiones HTTP y de procesar las respuestas del servidor, almacenándolas en un registro logístico para su revisión posterior. Esta capacidad permitirá un análisis detallado de la interacción entre el cliente y el servidor, facilitando una comprensión más profunda del flujo de datos y las operaciones de red.

<p align="center">
  <img src="https://github.com/gotaluism/ProyectoTelematica/assets/139718909/6a2a9f7e-c874-4cbc-b7a6-41e3cb278f92" alt="Diagrama de arquitectura del proyecto">
</p>


## Pre requisitos


## Desarrollo
### Servidor HTTP Proxy + Balanceador de Carga
El servidor se encarga de interceptar las peticiones de los clientes, reenviarlas a servidores destino seleccionados mediante el algoritmo de Round Robin, procesar las respuestas y retornarlas a los clientes. Se han implementado los siguientes aspectos:
- Escucha peticiones en el puerto 8080 y procesa solicitudes HTTP/1.1.
- Implementa los métodos GET y HEAD.
- Modifica las peticiones para enviarlas al servidor destino de forma adecuada.
- Implementa un archivo de log para registrar todas las peticiones y respuestas.
- Permite la caché de recursos solicitados por los clientes, almacenándolos en disco y estableciendo un Time To Live (TTL) configurable.

### Aplicación Cliente HTTP
La aplicación cliente permite realizar peticiones HTTP a cualquier servidor HTTP, incluido el servidor HTTP Proxy + Balanceador de Carga.
- `py cliente.py log.log url`
  - url: URL del recurso que deseamos solicitar.
  - Permite registrar todas las peticiones realizadas en un archivo de log, que incluye información sobre la fecha, hora, tipo de solicitud HTTP y respuesta recibida del servidor/proxy. 
  - La aplicación cliente puede realizar peticiones utilizando los métodos GET, HEAD y POST.
  - Ofrece la funcionalidad de caché de recursos solicitados, con la capacidad de eliminar completamente el caché mediante el comando flush.

### Servidores Web
Los servidores web se realizaron con Apache, cada servidor tiene una página estática en la cual hay un identificador con el cual se reconoce cuál de los 3 servidores web es. El servidor proxy es el que se encarga de redireccionar a uno de los 3 servidores web, no obstante, si deseas acceder a uno de los servidores web por aparte, puedes copiar la siguiente dirección en el buscador de tu preferencia:
- `ipPublicaDeUnoDeLosServidoresWeb/test/testt.html`
## Diagrama de flujo del proyecto
![Diagrama flujo telematica drawio](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/92fb3079-6249-4685-a67b-9ecd1448f9a2)

## Aspectos Logrados y No Logrados
### Aspectos Logrados : ✔️
- Implementación funcional del servidor HTTP Proxy + Balanceador de Carga y la aplicación cliente.
- Cumplimiento de los requisitos especificados, incluyendo el procesamiento de solicitudes HTTP/1.1, la implementación de métodos GET y HEAD y la modificación de las peticiones para enviar el encabezado pedido.
- Implementación del caché (verifica primero si la respuesta está en el caché y si está devuelve esa).
- Implementación del archivo log en donde se almacenan las solicitudes y las respuestas.
- Mensajes con código de estado (error 500 en caso de solicitar un método no implementado).
- Uso del algoritmo Round Robin para el balanceo de carga.
- Implementación completa de la aplicación cliente con todos sus requerimientos, log, caché, uso de métodos de GET, HEAD e implementación de método POST, comando flush.

### Aspectos No Logrados : ❌

## Conclusiones
El proyecto ha permitido adquirir conocimientos sólidos en el diseño y desarrollo de aplicaciones concurrentes en red, así como en el manejo de peticiones HTTP y la implementación de servidores proxy y balanceadores de carga. Se han alcanzado los objetivos principales del proyecto, aunque quedan algunos aspectos que fueron complicados de desarrollar debido al impedimento que teníamos respecto al uso de algunas librerías.

## Referencias
- Python, R. (2023, 16 noviembre). Socket Programming in Python (Guide). https://realpython.com/python-sockets/
- Codeboard Club. (2021, 18 marzo). Setup Apache Server as forward proxy, reverse proxy & load balancer. Step by step implementation [Vídeo]. YouTube. https://www.youtube.com/watch?v=eshV2whJrqk
- GfG. (2022, 18 noviembre). TCP Server-Client implementation in C. GeeksforGeeks. https://www.geeksforgeeks.org/tcp-server-client-implementation-in-c/
- A Monk in Cloud . (2022, 31 octubre). Run Jenkins Behind Apache Reverse Proxy | Jenkins on HTTP Port 80 with Custom Domain Name | AWS Demo [Vídeo]. YouTube. https://www.youtube.com/watch?v=vF3TS4U6I_M
- Learning Software. (2023, 6 agosto). Setup Apache as a Reverse Proxy [Vídeo]. YouTube. https://www.youtube.com/watch?v=VEr-Gp86teY
- Beej’s Guide to Network Programming. (s. f.). https://beej.us/guide/bgnet/
- RFC 7230: Hypertext Transfer Protocol (HTTP/1.1): Message Syntax and Routing. (s. f.). IETF Datatracker. https://datatracker.ietf.org/doc/html/rfc7230
- Williams, L. (2024, 3 febrero). Round Robin Scheduling Algorithm with Example. Guru99. https://www.guru99.com/round-robin-scheduling-example.html

### Autores
- **Santiago Arias Higuita** - [SantiagoArias229](https://github.com/SantiagoArias229)
- **Luis Miguel Giraldo Gonzalez**  - [gotaluism](https://github.com/gotaluism)
- **Vanessa Vélez Restrepo** - [vavelezr](https://github.com/vavelezr)


