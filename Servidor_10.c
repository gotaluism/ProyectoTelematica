#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h> // Para manipulación de archivos
#include <time.h> // Para manejar el tiempo
#include <sys/stat.h> // Para usar la función stat
#include <sys/types.h> // Para usar los tipos de datos struct stat y mode_t
#include <openssl/evp.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 2048
#define LOG_FILE "proxy_log.txt" // Nombre del archivo de log
#define CACHE_DIR "./cache/" // Directorio para almacenar caché

const char* SERVERS[] = {"52.90.165.72", "54.147.177.237", "54.211.33.153"};
const int NUM_SERVERS = sizeof(SERVERS) / sizeof(SERVERS[0]);
int current_server_index = 0;
int TTL = 3600; // TTL predeterminado en segundos (1 hora)


void hash_url(const char* url, char* outputBuffer, size_t outputSize) {
    unsigned char hash[EVP_MAX_MD_SIZE]; // Buffer para el hash
    unsigned int lengthOfHash = 0; // La longitud del hash resultante

    // Crea e inicializa un contexto para el cálculo del hash
    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if(context != NULL && EVP_DigestInit_ex(context, EVP_sha256(), NULL)) {
        // Alimenta el contexto con la URL
        if(EVP_DigestUpdate(context, url, strlen(url))) {
            // Finaliza el cálculo del hash y almacena el resultado en 'hash'
            EVP_DigestFinal_ex(context, hash, &lengthOfHash);
        }
    }

    // Limpia el contexto
    EVP_MD_CTX_free(context);

    // Convierte el hash en una cadena hexadecimal
    for(unsigned int i = 0; i < lengthOfHash; ++i) {
        snprintf(&(outputBuffer[i*2]), outputSize-(i*2), "%02x", hash[i]);
    }
}


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
    char hashed_url[65]; // 64 caracteres para SHA-256 + 1 para el carácter nulo
    hash_url(url, hashed_url, sizeof(hashed_url));
    
    char filepath[2048];
    snprintf(filepath, sizeof(filepath), "%s%s.cache", CACHE_DIR, hashed_url);

    int cache_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cache_fd == -1) {
        perror("Error al abrir o crear el archivo de caché");
        return;
    }

    write(cache_fd, response, response_length);
    close(cache_fd);

    // Guardar el tiempo de expiración del caché en el mismo archivo para simplificar
    snprintf(filepath, sizeof(filepath), "%s%s.ttl", CACHE_DIR, hashed_url);
    cache_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (cache_fd == -1) {
        perror("Error al abrir o crear el archivo TTL");
        return;
    }
    time_t expiration_time = time(NULL) + ttl;
    dprintf(cache_fd, "%ld", expiration_time); 
    close(cache_fd);
}

int check_cache(const char* url, char* response_buffer, ssize_t* response_length) {
    char hashed_url[65];
    hash_url(url, hashed_url, sizeof(hashed_url));

    char filepath[2048];
    snprintf(filepath, sizeof(filepath), "%s%s.cache", CACHE_DIR, hashed_url);

    int cache_fd = open(filepath, O_RDONLY);
    if (cache_fd == -1) {
        return -1; // No hay caché
    }

    *response_length = read(cache_fd, response_buffer, MAX_BUFFER_SIZE - 1);
    close(cache_fd);

    if (*response_length <= 0) {
        return -1; // Error al leer el caché
    }

    // Verificar TTL
    snprintf(filepath, sizeof(filepath), "%s%s.ttl", CACHE_DIR, hashed_url);
    cache_fd = open(filepath, O_RDONLY);
    if (cache_fd == -1) {
        return -1; // Error al abrir TTL
    }
    
    char expiration_time_str[64];
    if (read(cache_fd, expiration_time_str, sizeof(expiration_time_str)) <= 0) {
        close(cache_fd);
        return -1; // Error al leer TTL
    }
    close(cache_fd);

    time_t expiration_time = atol(expiration_time_str);
    if (time(NULL) > expiration_time) {
        return -1; // Caché expirado
    }

    // Caché válido
    return 0;
}
void forward_request_to_server(int client_socket, const char* server_ip, char* client_request, ssize_t request_length){
    int web_server_socket;
    struct sockaddr_in web_server_addr;
    ssize_t sent_bytes, received_bytes;
    char buffer[MAX_BUFFER_SIZE];

    // Verificar si la respuesta está en caché
    char response_buffer[MAX_BUFFER_SIZE];
    ssize_t cached_response_length = 0; // Inicializar la longitud de la respuesta del caché a 0
    int cache_status = check_cache(client_request, response_buffer, &cached_response_length);
    
    if (cache_status == 0) { // Si el caché es válido y fue encontrado
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
        
        // Imprimir la dirección IP de destino actual (servidor al que el proxy conectará)

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

//Funcion para poder mandar el error 500 en caso de que el cliente mande cosas que no se han creado o las mande mal
void send_http_error(int client_socket) {
    const char* response =
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>500 Internal Server Error</h1></body></html>\r\n";

    send(client_socket, response, strlen(response), 0);
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

    // Encontrar la solicitud HTTP en el buffer
    char method[10]; // Suponemos que el método tiene una longitud máxima de 10 caracteres
    if (sscanf(buffer, "%9s", method) != 1) {
        perror("Error al analizar la solicitud HTTP");
        close(client_socket);
        return;
    }
    
    // Verificar el método HTTP
    if (strcmp(method, "GET") == 0) {
        printf("Se recibió una solicitud GET\n");

        // Imprimir la solicitud recibida del cliente
        printf("Solicitud recibida de: %s\n", client_ip);
        printf("Solicitud HTTP:\n%s\n", buffer);


        // Imprimir la dirección IP de destino actual (servidor al que el proxy conectará)
        printf("Conectando con el servidor de destino: %s\n", SERVERS[current_server_index]);

        forward_request_to_server(client_socket, SERVERS[current_server_index], buffer, bytes_received);
        current_server_index = (current_server_index + 1) % NUM_SERVERS;
    } else if (strcmp(method, "POST") == 0) {
        printf("Se recibió una solicitud POST\n");
        
        // Imprimir la solicitud recibida del cliente
        printf("Solicitud recibida de: %s\n", client_ip);
        printf("Solicitud HTTP:\n%s\n", buffer);


        // Imprimir la dirección IP de destino actual (servidor al que el proxy conectará)
        printf("Conectando con el servidor de destino: %s\n", SERVERS[current_server_index]);

        forward_request_to_server(client_socket, SERVERS[current_server_index], buffer, bytes_received);
        current_server_index = (current_server_index + 1) % NUM_SERVERS;
    } else if (strcmp(method, "HEAD") == 0) {
        printf("Se recibió una solicitud HEAD\n");
        
        // Imprimir la solicitud recibida del cliente
        printf("Solicitud recibida de: %s\n", client_ip);
        printf("Solicitud HTTP:\n%s\n", buffer);


        // Imprimir la dirección IP de destino actual (servidor al que el proxy conectará)
        printf("Conectando con el servidor de destino: %s\n", SERVERS[current_server_index]);

        forward_request_to_server(client_socket, SERVERS[current_server_index], buffer, bytes_received);
        current_server_index = (current_server_index + 1) % NUM_SERVERS;
    } else {
        printf("Método HTTP no reconocido: %s\n", method);
        send_http_error(client_socket); // Enviar error 500 si el método no es soportado
        close(client_socket);
        return;
    }


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
