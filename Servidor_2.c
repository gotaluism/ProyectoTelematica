#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 2048

// Lista de direcciones IP de los servidores web en AWS
//RECUERDEN ESTOS PUERTOS CAMBIAN CADA X TIEMPO, ACTUALIZARLOSSSSS
const char* SERVERS[] = {"52.87.175.99", "54.167.77.176", "34.224.71.81"};
const int NUM_SERVERS = sizeof(SERVERS) / sizeof(SERVERS[0]);
int current_server_index = 0;

void handle_client(int client_socket) {
    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytes_received;

    // Recibir la solicitud del cliente
    bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received < 0) {
        perror("Error al recibir la solicitud del cliente");
        close(client_socket);
        return;
    }

    // Conectar con el servidor web
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error al crear el socket del servidor web");
        close(client_socket);
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    inet_pton(AF_INET, SERVERS[current_server_index], &server_addr.sin_addr);

    if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor web");
        close(client_socket);
        close(server_socket);
        return;
    }

    // Enviar la solicitud al servidor web
    send(server_socket, buffer, bytes_received, 0);

    // Recibir la respuesta del servidor web
    ssize_t bytes_sent;
    while ((bytes_received = recv(server_socket, buffer, sizeof(buffer), 0)) > 0) {
        bytes_sent = send(client_socket, buffer, bytes_received, 0);
        if (bytes_sent < 0) {
            perror("Error al enviar la respuesta al cliente");
            break;
        }
    }

    // Cerrar los sockets
    close(client_socket);
    close(server_socket);

    // Actualizar el índice del servidor para balanceo de carga
    current_server_index = (current_server_index + 1) % NUM_SERVERS;
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
