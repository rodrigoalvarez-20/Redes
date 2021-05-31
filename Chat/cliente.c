#include "chat.h"


int main() {
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in local, remota;
    pthread_t send, receive;
    chat local_params, remote_params;

    verifySocketConnection(client_socket);

    //Estructura local --> Recibe los mensajes
    local.sin_family = AF_INET;
    local.sin_port = htons(CLIENT_PORT);
    local.sin_addr.s_addr = INADDR_ANY;

    int lbind = bind(client_socket, (struct sockaddr *)&local, sizeof(local));

    verifyBind(lbind);

    //Remota --> Enviar mensajes
    remota.sin_family = AF_INET;
    remota.sin_port = htons(SERVER_PORT);
    remota.sin_addr.s_addr = inet_addr(DEST_IP);

    remote_params.remota = remota;
    remote_params.socket = client_socket;

    local_params.remota = local;
    local_params.socket = client_socket;

    pthread_create(&send, NULL, &sendMessage, &remote_params);
    pthread_create(&receive, NULL, &receiveMessage, &local_params);

    pthread_join(send, NULL);
    pthread_join(receive, NULL);

    close(client_socket);

    return 0;
}