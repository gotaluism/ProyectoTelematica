#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> // Para manipulación de archivos
#include <time.h> // Para manejar el tiempo
#include <sys/stat.h> // Para usar la función stat
#include <sys/types.h> // Para usar los tipos de datos struct stat y mode_t


#define PORT 8080
#define MAX_BUFFER_SIZE 2048
#define LOG_FILE "proxy_log.txt" // Nombre del archivo de log
#define CACHE_DIR "./cache/" // Directorio para almacenar caché

const char* SERVERS[] = {"54.236.108.182", "3.89.252.247", "35.175.199.238"};
const int NUM_SERVERS = sizeof(SERVERS) / sizeof(SERVERS[0]);
int current_server_index = 0;
int TTL = 3600; // TTL predeterminado en segundos (1 hora)


void log_request_response(const char* request, ssize_t request_length, const char* response, ssize_t response_length) {
    // Abrir el archivo de log en modo de escritura y agregar (append)
    int log_fd = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (log_fd == -1) {
        perror("Error al abrir el archivo de log");
        return;
    }

    // Escribir la solicitud en el archivo de log
    write(log_fd, "Request:\n", strlen("Request:\n"));
    write(log_fd, request, request_length);

    // Escribir la respuesta en el archivo de log
    write(log_fd, "\n\nResponse:\n", strlen("\n\nResponse:\n"));
    write(log_fd, response, response_length);

    // Cerrar el archivo de log
    close(log_fd);
}

void cache_response(const char* url, const char* response, ssize_t response_length, int ttl) {
    char filename[1024];
    snprintf(filename, sizeof(filename), "%s%s.cache", CACHE_DIR, url);

    int cache_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cache_fd == -1) {
        perror("Error al abrir o crear el archivo de caché");
        return;
    }

    write(cache_fd, response, response_length);
    close(cache_fd);

    // Guardar el tiempo de expiración del caché en un archivo separado
    time_t expiration_time = time(NULL) + ttl;
    snprintf(filename, sizeof(filename), "%s%s.ttl", CACHE_DIR, url);
    cache_fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cache_fd == -1) {
        perror("Error al abrir o crear el archivo TTL");
        return;
    }
    char expiration_time_str[64];
    snprintf(expiration_time_str, sizeof(expiration_time_str), "%ld", expiration_time);
    write(cache_fd, expiration_time_str, strlen(expiration_time_str));
    close(cache_fd);
}

int check_cache(const char* url, char* response_buffer) {
    char filename[1024];
    snprintf(filename, sizeof(filename), "%s%s.cache", CACHE_DIR, url);

    int cache_fd = open(filename, O_RDONLY);
    if (cache_fd == -1) {
        // El archivo de caché no existe
        return -1;
    }

    // Leer la respuesta del caché
    ssize_t bytes_read = read(cache_fd, response_buffer, MAX_BUFFER_SIZE);
    close(cache_fd);

    if (bytes_read <= 0) {
        // Error al leer el caché
        return -1;
    }

    // Verificar el tiempo de expiración del caché
    snprintf(filename, sizeof(filename), "%s%s.ttl", CACHE_DIR, url);
    cache_fd = open(filename, O_RDONLY);
    if (cache_fd == -1) {
        // Error al abrir el archivo TTL
        return -1;
    }
    char expiration_time_str[64];
    bytes_read = read(cache_fd, expiration_time_str, sizeof(expiration_time_str));
    close(cache_fd);

    if (bytes_read <= 0) {
        // Error al leer el archivo TTL
        return -1;
    }

    time_t expiration_time = strtol(expiration_time_str, NULL, 10);
    time_t current_time = time(NULL);
    if (current_time > expiration_time) {
        // El caché ha expirado
        return -1;
    }

    // El caché es válido
    return bytes_read;
}
void forward_request_to_server(int client_socket, const char* server_ip, char* client_request, ssize_t request_length){
    int web_server_socket;
    struct sockaddr_in web_server_addr;
    ssize_t sent_bytes, received_bytes;
    char buffer[MAX_BUFFER_SIZE];

    // Verificar si la respuesta está en caché
    char response_buffer[MAX_BUFFER_SIZE];
    ssize_t cached_response_length = check_cache(client_request, response_buffer);
    if (cached_response_length > 0) {
        // Enviar la respuesta almacenada en caché al cliente
        send(client_socket, response_buffer, cached_response_length, 0);
        printf("Respuesta obtenida del caché\n");
        return;
    }

    // Crear un socket para conectarse al servidor web
    web_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (web_server_socket == -1) {
        perror("Error al crear el socket para el servidor web");
        close(client_socket);
        return;
    }

    // Configurar la dirección del servidor web
    web_server_addr.sin_family = AF_INET;
    web_server_addr.sin_port = htons(80); // El puerto HTTP estándar
    if (inet_pton(AF_INET, server_ip, &web_server_addr.sin_addr) <= 0) {
        perror("Dirección IP del servidor web inválida o error");
        close(web_server_socket);
        close(client_socket);
        return;
    }

    // Conectarse al servidor web
    if (connect(web_server_socket, (struct sockaddr*)&web_server_addr, sizeof(web_server_addr)) < 0) {
        perror("Error al conectarse al servidor web");
        printf("No se pudo conectar con el servidor de destino: %s\n", server_ip);
        close(web_server_socket);
        close(client_socket);
        return;
    } else {
        printf("Conexión exitosa con el servidor de destino: %s\n", server_ip);
    }

    // Enviar la solicitud al servidor web
    sent_bytes = send(web_server_socket, client_request, request_length, 0);
    if (sent_bytes < 0) {
        perror("Error al enviar la solicitud al servidor web");
        close(web_server_socket);
        close(client_socket);
        return;
    }

    // Recibir la respuesta del servidor web y mostrarla
    ssize_t total_received_bytes = 0;
    do {
        received_bytes = recv(web_server_socket, buffer, sizeof(buffer), 0);
        if (received_bytes < 0) {
            perror("Error al recibir la respuesta del servidor web");
            break;
        } else if (received_bytes == 0) {
            printf("El servidor web cerró la conexión\n");
            break;
        }

        // Imprimir la respuesta en la consola
        fwrite(buffer, sizeof(char), received_bytes, stdout);

        // Registrar la solicitud y respuesta en el archivo de log
        log_request_response(client_request, request_length, buffer, received_bytes);

        total_received_bytes += received_bytes;
    } while (received_bytes > 0);

    // Cerrar el socket del servidor web
    close(web_server_socket);

    // Almacenar la respuesta en caché si es necesario
    if (cached_response_length <= 0) { // Solo si la respuesta no estaba en caché
        cache_response(client_request, buffer, total_received_bytes, TTL);
    }
}
void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_received;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Obtener la dirección del cliente conectado
    getpeername(client_socket, (struct sockaddr*)&client_addr, &client_addr_len);

    // Convertir la dirección IP de origen a una cadena
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    // Recibir la solicitud del cliente
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error al recibir la solicitud del cliente");
        close(client_socket);
        return;
    }

    // Imprimir la solicitud recibida del cliente
    printf("Solicitud recibida de: %s\n", client_ip);
    printf("Solicitud HTTP:\n%s\n", buffer);

    // Crear un nuevo socket para conectarse al servidor web de destino
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket para el servidor web de destino");
        close(client_socket);
        return;
    }

    // Configurar la dirección del servidor web de destino
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // Puerto HTTP estándar
    inet_pton(AF_INET, SERVERS[current_server_index], &server_addr.sin_addr);

    // Conectarse al servidor web de destino
    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectarse al servidor web de destino");
        close(server_socket);
        close(client_socket);
        return;
    }

    // Enviar la solicitud al servidor web de destino sin modificarla
    ssize_t sent_bytes = send(server_socket, buffer, bytes_received, 0);
    if (sent_bytes != bytes_received) {
        perror("Error al enviar la solicitud al servidor web");
        close(server_socket);
        close(client_socket);
        return;
    }

    // Recibir la respuesta del servidor web y enviarla de vuelta al cliente
    ssize_t received_bytes;
    do {
        received_bytes = recv(server_socket, buffer, sizeof(buffer), 0);
        if (received_bytes < 0) {
            perror("Error al recibir la respuesta del servidor web");
            break;
        } else if (received_bytes == 0) {
            printf("El servidor web cerró la conexión\n");
            break;
        }

        // Enviar la respuesta recibida del servidor web de vuelta al cliente
        ssize_t sent_bytes = send(client_socket, buffer, received_bytes, 0);
        if (sent_bytes != received_bytes) {
            perror("Error al enviar la respuesta al cliente");
            break;
        }
    } while (received_bytes > 0);

    // Cerrar los sockets
    close(server_socket);
    close(client_socket);
}
int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Crear el socket del servidor
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del servidor");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Enlazar el socket a la dirección y puerto
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al enlazar el socket al puerto");
        exit(EXIT_FAILURE);
    }
    // Escuchar conexiones entrantes
    if (listen(server_socket, 5) < 0) {
        perror("Error al escuchar conexiones entrantes");
        exit(EXIT_FAILURE);
    }

    printf("Servidor Proxy + Balanceador de Carga escuchando en el puerto %d...\n", PORT);

    // Aceptar conexiones y manejarlas
    while (1) {
        // Aceptar la conexión del cliente
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            perror("Error al aceptar la conexión del cliente");
            continue;
        }
        // Manejar la solicitud del cliente
        handle_client(client_socket);
    }

    // Cerrar el socket del servidor (esto nunca se alcanzará en este ejemplo)
    close(server_socket);

    return 0;
}
