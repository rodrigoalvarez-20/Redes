#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define CLIENT_PORT 8081
#define SERVER_PORT 8080
#define DEST_IP "192.168.100.41"


typedef struct s_chat {
    int socket;
    struct sockaddr_in remota;
} chat;


void *sendMessage(void *data){
    chat *params = (chat *) data;
    char msj[100];
    while(1){
        printf("Introduce el mensaje: \n");
        fgets(msj, sizeof(msj), stdin);

        int tam = sendto(params->socket, msj, strlen(msj) + 1, 0, (struct sockaddr *)&(params->remota), sizeof(params->remota));

        if (tam == -1) {
            //Error al enviar los datos
            perror("Ha ocurrido un error al enviar los datos al servidor\n");
            exit(-1);
        }
    }

}

void *receiveMessage(void *data){
    //Estar escuchando y mostrar el mensaje
    chat *params = (chat *) data;
    char msjRecv[512];

    while(1){
        int lrecv = sizeof(params->remota);
        int tam = recvfrom(params->socket, msjRecv, 512, 0, (struct sockaddr *)&(params->remota), &lrecv);
        if(tam == -1){
            perror("Mensaje no procesable\n");
            exit(-1);
        }
        
        printf("Mensaje nuevo: %s\n", msjRecv);
        printf("Introduce el mensaje: \n");
    }
    
}

void verifySocketConnection(int socket){
    if(socket == -1){
        perror("Ha ocurrido un error al crear el socket\n");
        exit(-1);
    }
    printf("Socket creado con exito\n");
}

void verifyBind(int lbdind){
    if(lbdind == -1){
        perror("Ha ocurrido un error al hacer el bind\n");
        exit(-1);
    }

    printf("Bind hecho con exito\n");

}