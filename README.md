<h1 align="center">ST0255 TELEMÁTICA</h1>
<h1 align="center">PROYECTO #1</h1>
<h1 align="center">TEMÁTICA: PROGRAMACIÓN EN RED A PARTIR DE LA CREACIÓN DE UN PROXY INVERSO + BALANCEADOR DE CARGA Y CLIENTE</h1>

## Video explicativo
Link del video explicativo: https://youtu.be/7tvadK4IWd4


## Tabla de Contenidos
- [Introducción](#introducción)
- [Prerequisitos](#prerequisitos)
- [Desarrollo](#desarrollo)
  - [Servidor HTTP Proxy + Balanceador de Carga](#servidor-http-proxy--balanceador-de-carga)
  - [Servidores Web](#servidores-web)
  - [Aplicación Cliente HTTP](#aplicación-cliente-http)
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


## Prerequisitos

Para llevar a cabo este proyecto con éxito, es esencial tener los siguientes prerrequisitos:

1. **Conocimientos en Programación de Redes:**
   - Entender el modelo cliente-servidor y el funcionamiento de las aplicaciones en red.
   - Conocimientos fundamentales de TCP/IP y cómo se comunican las aplicaciones a través de Internet.

2. **Experiencia en Programación:**
   - Competencia en Python para el desarrollo del cliente HTTP. Familiaridad con la sintaxis básica y las bibliotecas de red, como `socket` , si se considera su uso.
   - Habilidades en el lenguaje C para la implementación del servidor, dado que el servidor proxy y el balanceador de carga deben ser programados en C sin usar librerías externas para el procesamiento HTTP.

3. **Manejo de Apache:**
   - Experiencia en configurar y administrar servidores Apache. Conocimientos sobre cómo desplegar aplicaciones web y configurar aspectos como virtual hosts y módulos de Apache.

4. **Uso de la API de Sockets:**
   - Familiaridad con la API de Sockets de Berkeley, especialmente los sockets de tipo Stream (TCP) que son los que se usarán principalmente en este proyecto.
   - Capacidad para implementar comunicaciones basadas en sockets en C y Python.

5. **Entorno de Desarrollo:**
   - Acceso a un entorno Linux para el desarrollo y pruebas del servidor, ya que la mayoría de los servidores web y de aplicaciones operan sobre sistemas UNIX-like.
   - Configuración de un entorno Python para el desarrollo del cliente, incluyendo un editor o IDE que soporte Python y herramientas de debug.

6. **Herramientas de Versionado:**
   - Experiencia con sistemas de control de versiones como Git para manejar las versiones del código y colaborar entre los miembros del equipo.

7. **Acceso a Servidores en la Nube:**
   - Disponibilidad de servidores en la nube, como los proporcionados por AWS, para desplegar y probar los componentes del proyecto. Se requiere configurar y administrar máquinas virtuales para el proxy, el balanceador de carga, y los servidores web Apache.



## Desarrollo
### Servidor HTTP Proxy + Balanceador de Carga
Nuestro servidor proxy fue desarrollado en el lenguaje de programación C, con el uso de la librería de Socket de Berkeley.

El servidor se encarga de interceptar las peticiones de los clientes, reenviarlas a servidores destino seleccionados mediante el algoritmo de Round Robin, procesar las respuestas y retornarlas a los clientes. Se han implementado los siguientes aspectos:
- Escucha peticiones en el puerto 8080 y procesa solicitudes HTTP/1.1.
- Implementa los métodos GET y HEAD.
- Modifica las peticiones para enviarlas al servidor destino de forma adecuada.
- Implementa un archivo de log para registrar todas las peticiones y respuestas.
- Permite la caché de recursos solicitados por los clientes, almacenándolos en disco y estableciendo un Time To Live (TTL) configurable.

#### Para tener en cuenta
Para el correcto funcionamiento de nuestro servidor, debemos cambiar en nuestro código las ips de nuestros 3 servidores web
![image](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/041c3aee-f049-493b-a85b-5d741fbb22b8)
- Para estso haremos lo siguiente:
- Nos dirigimos a cada instancia de nuestros servidores web, le damos en conectar y copiamos la ip pública que nos entregan
- ![image](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/4421228b-3c4e-4719-8b9b-ca7e6b7ca542)
- Posteriormente en nuestro código vamos reemplazando cada ip (primero va la ip servidor 1, después 2 y por ultimo la del servidor 3)
- Ya todo estaría listo para el funcionamiento


#### Hacer cambios en el código del servidor
En caso de querer hacer cambios en el código de nuestro servidor haremos lo siguiente:
 `sudo nano Servidor.c`
- En este punto ya podemos editar y guardamos con Ctrl+O y nos salimos con Ctrl+X

- Para compilar nuestros cambios realizamos el siguiente comando:
`gcc Servidor.c -o servidor -lcrypto`

- Para su ejecución nos dirigimos a nuestra instancia "My First Instance" en AWS y le damos en conectar. Posterior a esto copiamos el siguiente comando
`sudo ./servidor`
- Acá ya está en funcionamiento nuestro servidor proxy y se vería algo así:
![image](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/66d737b5-1f1c-4be3-898a-0eef79d5dee0)

#### Para acceder al caché y a los archivos que nos cree
- Nos dirigimos a la consola de nuestro servidor y copiamos el comando `ls` para verificar que estamos en la misma ruta de nuestro caché.
- Nos debería de aparecer la carpeta "caché", accedemos a ella con el comando `cd cache`.
- Dentro de esta volvemos a copiar el comando `ls` y podremos verificar que allí se encuentran todos nuestros archivos .cache y su respectivo archivo . ttl.
![image](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/1cef72b8-3853-414e-8a4d-a32696e8ab8d)


#### Para acceder y ver los registros del log
- En la misma ruta en donde encontramos nuestro servidor, carpeta caché. etc. Encontramos nuestro archivo llamado "proxy_log.txt".
- Para visualizar el contenido de nuestro archivo copiamos `cat proxy_log.txt`.
![image](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/5f14ff5e-62e3-43fc-b3d6-10f6161c01c2)

El contenido de la respuesta se encuentra encriptado con gzip.

#### Probar su funcionamiento (sin tener en cuenta el cliente)

- Nos dirigmos a Postman y copiamos la siguiente url:
```
http://18.235.48.199:8080/test/testt.html

```
- En el método ponemos "GET" o "HEAD" y le damos en "Send".



### Servidores Web
Los servidores web se realizaron con Apache, cada servidor tiene una página estática en la cual hay un identificador con el cual se reconoce cuál de los 3 servidores web es. El servidor proxy es el que se encarga de redireccionar a uno de los 3 servidores web, no obstante, si deseas acceder a uno de los servidores web por aparte, puedes copiar la siguiente dirección en el buscador de tu preferencia:
- `ipPublicaDeUnoDeLosServidoresWeb/test/testt.html`

### Aplicación Cliente HTTP
La aplicación cliente debe permitir realizar peticiones HTTP a cualquier servidor HTTP, incluido el servidor HTTP Proxy + Balanceador de Carga.
  - Permite registrar todas las peticiones realizadas en un archivo de log, que incluye información sobre la fecha, hora, tipo de solicitud HTTP y respuesta recibida del servidor/proxy. 
  - La aplicación cliente puede realizar peticiones utilizando los métodos GET, HEAD y POST.
  - Ofrece la funcionalidad de caché de recursos solicitados, con la capacidad de eliminar completamente el caché mediante el comando flush.
#### ¿Como se desarrolló? :
- Se uso `python` con la versión `3.10.9`
- El archivo que contiene el codigo de la aplicación cliente el cual esta 100% comentado se llama `cliente.py`

- El codigo se ejecuta de la siguiente manera :
  
```
py cliente.py log.log  URL PUERTO METODO

```
Si se escribe mal el programa indica la correcta escritura:
![image](https://github.com/gotaluism/ProyectoTelematica/assets/139718909/22aa44cd-58ad-4733-ba8e-25c738c0a269)

- Para ejecutar el comando flush
```
py cliente.py flush

```
El comando avisa que la cache ha sido limpiada: 

![image](https://github.com/gotaluism/ProyectoTelematica/assets/139718909/aa5e53a3-374d-46a1-bfb3-52313397b6d3)

- Así se observa una petición en el log:
  
  ![image](https://github.com/gotaluism/ProyectoTelematica/assets/139718909/e7dea5bc-3bac-4fe8-baad-9a5082895ad0)
- Así se observa una petición en el cache:
  
  ![image](https://github.com/gotaluism/ProyectoTelematica/assets/139718909/f6793f90-4bf2-4e97-84ee-93423473452d)

  #### Ejemplos de como probar el codigo:
- Para una imagen (notese que si el puerto no está definido en la url el asigna uno por defecto):
```
py cliente.py log.log  https://media.es.wired.com/photos/650b2a2e72d73ca3bd5ef0cc/16:9/w_2560%2Cc_limit/Business-OpenAI-Dall-E-3-heart.jpg  GET

```
- Para un archivo:
```
py cliente.py log.log https://ministeriodeeducacion.gob.do/docs/biblioteca-virtual/4tXN-heidegger-martin-que-es-la-filosofiapdf.pdf GET

```
- Para el proxy elaborado (cuando el metodo es POST debe de ir una palabra para que el servidor la reciba):
```
py cliente.py log.log  http://18.235.48.199:8080/test/testt.html POST hola

```


## Diagrama de flujo del proyecto
De acuerdo al desarrollo esperado y explicado previamente, se espera que el proyecto se comporte de acuerdo al siguiente diagrama de flujo:

  ![Diagrama flujo telematica drawio](https://github.com/gotaluism/ProyectoTelematica/assets/88945658/92fb3079-6249-4685-a67b-9ecd1448f9a2)

## Aspectos Logrados y No Logrados
A continuación se presenta una tabla de aspectos logrados y no logrados con respecto al proyecto la cual se ira actualizando durante el desarrollo para la verificación de el logro de los mismos.

| Aspecto |    Logrado| No logrado | Comentario|
|-----------|-----------|-----------|-----------|
| El cliente, HTTP Proxy + Balanceador de Carga + Servidor Web se comunican a través de los mensajes definidos para el protocolo HTTP/1.1.   | X    |       |       |
| El servidor HTTP Proxy + Balanceador de Carga  permite interceptar una petición HTTP/1.1 y enviarla a un conjunto de servidores web que opera a nivel de back end.  | X      |       |       |
| El servidor HTTP Proxy + Balanceador de carga está escrito en lenguaje de programación C    | X      |      |       |
| El servidor escucha peticiones en el puerto 8080. Una vez reciba la petición de un cliente    | X      |       |     |
| La aplicación HTTP Proxy + Balanceador de Carga implementa un archivo de “log” donde se registran todas las peticiones que recibe.    | X      |       |      |
| El servidor HTTP Proxy + Balanceador de Carga es responsable de implementar los siguientes métodos GET Y HEAD   | X      |     |       |
| La función de proxy debe permitir el caché para los diferentes recursos que se soliciten por parte de los clientes.    | X      |       |      |
| Mensajes con código de estado (error 500 en caso de solicitar un método no implementado)   | X      |       |       |
| Uso el proxy hace uso del algoritmo Round Robin para el balanceo de carga   | X      |       |       |
| El servidor proxy se ejecuta de la siguiente manera /httproxy <port> </path/log.log>   | X      |       |       |
| La aplicación cliente se ejecuta de la siguiente forma ./httpclient </path/log.log> <url:port>  | X      |       |       |
| La aplicación cliente cuenta con un archivo log donde se registran las peticiones en la forma <date> <time> <http_request> <http_response>   | X      |       |       |
| La aplicación cliente, puede consultar cualquier recurso web vía HTTP   | X      |      |      |
| La aplicación cliente permite alamcenar archivos de la web como imágenes (png, jpeg), documentos (pdf,docx,ppt,xls), videos (mpeg, mov), etc | X      |       |       |
| La aplicación cliente permite visualizar en consola los textos extraidos de la web  | X      |      |      |
| La aplicación cliente es capaz de realizarpeticiones empleando los métodos GET, HEAD y POST    |  X   |      |      |
| La alpicación cliente cuenta con un cache persistente el cual permite almacenar las ultimas consultas realizadas y ademas permite revisar si ya se han hecho para cargarlas desde este cache  | X      |   |   |
| La aplicación cliente cuenta con un comando flush para eliminar el cache   | X      |      |      |




## Conclusiones
- El sistema de caché hace que las cosas vayan más rápido al guardar temporalmente las respuestas comunes, así el servidor no tiene que hacer el mismo trabajo una y otra vez. El uso de un algoritmo de hash para generar identificadores únicos y un control de tiempo de vida (TTL) garantiza una gestión eficiente y efectiva de las entradas en caché.
- El registro de logs es como la "agenda" de nuestro servidor, donde se anotan todas las solicitudes que llegan y las respuestas que enviamos. Es como llevar un registro de lo que pasa en la aplicación. Esto nos ayuda a solucionar problemas más fácilmente y a entender cómo se comporta el sistema con los usuarios. Así podemos asegurarnos de que todo funciona bien y mejorar en el futuro.
- Para poder navegar en internet con el cliente se debe de envolver el socket con SSL y que así los sitios seguros acepten la conexión.
- Al servir documentos HTML en UTF-8, es crucial especificar correctamente la codificación en las cabeceras HTTP y en las etiquetas meta del HTML de lo contrario va a dar un error no relacionado con respecto a los caracteres llevando a una incorrecta visualización.
- Se he aprendido el uso del manejo de los servidores web, los cuales mediante apache permitieron renderizar las paginas estáticas.
- Se obtuvieron muchos conocimientos solidos acerca de la estructura para construir una aplicación concurrente de red, desde los sockets, hasta los diversos servidores que se construyeron para llevar exitosamente este proyecto.

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


