#include "chat.h"

//Servidor

int main() {
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servidor, cliente;
    chat local_params, remote_params;
    pthread_t send, receive;

    verifySocketConnection(server_socket);

    //Estructura servidor -- Local
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(SERVER_PORT);
    servidor.sin_addr.s_addr = INADDR_ANY;

    int lbind = bind(server_socket, (struct sockaddr *)&servidor, sizeof(servidor));

    verifyBind(lbind);

    //Crear la conexion para la remota

    cliente.sin_family = AF_INET;
    cliente.sin_port = htons(CLIENT_PORT);
    cliente.sin_addr.s_addr = inet_addr(DEST_IP);

    //Thread
    local_params.remota = servidor; //Para escuchar tengo que ocupar la local
    local_params.socket = server_socket;

    remote_params.remota = cliente;
    remote_params.socket = server_socket;

    pthread_create(&receive, NULL, &receiveMessage, &local_params);
    pthread_create(&send, NULL, &sendMessage, &remote_params);
    
    pthread_join(receive, NULL);
    pthread_join(send, NULL);

    close(server_socket);

    return 0;
}