#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 2048

// Lista de direcciones IP de los servidores web en AWS
//RECUERDEN ESTOS PUERTOS CAMBIAN CADA X TIEMPO, ACTUALIZARLOSSSSS
const char* SERVERS[] = {"3.89.56.244", "54.89.10.146", "54.221.41.180"};
const int NUM_SERVERS = sizeof(SERVERS) / sizeof(SERVERS[0]);
int current_server_index = 0;



// Función para conectar con el servidor web y reenviar la solicitud
void forward_request_to_server(int client_socket, const char* server_ip, char* client_request, ssize_t request_length){
    int web_server_socket;
    struct sockaddr_in web_server_addr;
    ssize_t sent_bytes, received_bytes;
    char buffer[MAX_BUFFER_SIZE];

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
    } while (received_bytes > 0);

    // Cerrar el socket del servidor web
    close(web_server_socket);
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
    } else if (strcmp(method, "POST") == 0) {
        printf("Se recibió una solicitud POST\n");
    } else if (strcmp(method, "HEAD") == 0) {
        printf("Se recibió una solicitud HEAD\n");
    } else {
        printf("Método HTTP no reconocido: %s\n", method);
    }

    // Imprimir la dirección IP de origen del cliente
    printf("Solicitud recibida de: %s\n", client_ip);

    // Imprimir la dirección IP de destino actual (servidor al que el proxy conectará)
    printf("Conectando con el servidor de destino: %s\n", SERVERS[current_server_index]);

    // Conectar con el servidor web y continuar el flujo de la función...
    // Tu código para conectar con el servidor web y enviar la solicitud
    // ...
    forward_request_to_server(client_socket, SERVERS[current_server_index], buffer, bytes_received);
    current_server_index = (current_server_index + 1) % NUM_SERVERS;
    // Cerrar el socket del cliente después de reenviar la solicitud y recibir la respuesta del servidor
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
