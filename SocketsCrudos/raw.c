#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <string.h>

unsigned char 
    MAC_Origen[6], 
    NET_MASK[4], 
    IP_ADDR[4],
    frameToSend[1514],
    rFrame[1514],
    ETH_TYPE[2] = {0x0c, 0x0c},
    MAC_Broad[6] = {0xff, 0xff,0xff,0xff,0xff,0xff},
    FRAME_SEND[2] = {0x00, 0x01},
    FRAME_RECV[2] = {0x00, 0x02},
    P_TYPE[2] = {0x08, 0x00},
    HW_LEN[1] = {0x06},
    P_LEN[1] = {0x04},
    HW_TYPE[2] = {0x00, 0x01};
int interface_index;


void getData(int);
void displayMac(unsigned char[], int);
void displayDir(unsigned char[], int);
void setFrame(unsigned char *);
void sendFrame(int, unsigned char *);
void receiveFrame(int, unsigned char*);
void displayFrame(unsigned char *, int);


int main(){
    int packet_socket;

    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if((packet_socket == -1)){
        perror("Ha ocurrido un error al crear el socket\n");
        exit(-1);
    }

    getData(packet_socket);
    setFrame(frameToSend);
    sendFrame(packet_socket, frameToSend);
    receiveFrame(packet_socket, rFrame);    
    
    close(packet_socket);

    return 0;
}

void getData(int socket){
    struct ifreq nic;
    unsigned char interface[20];
    printf("Inserta el nombre de la interfaz de red a ocupar: ");
    scanf("%s", interface);
    strcpy(nic.ifr_name, interface);
    if(ioctl(socket, SIOCGIFINDEX, &nic) == -1){
        perror("Ha ocurrido un error obtener el indice de la interfaz\n");
        exit(-1);
    }
    interface_index = nic.ifr_ifindex;
    //Direccion MAC de origen

    if(ioctl(socket, SIOCGIFHWADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la mac de origen IP\n");
        exit(-1);
    }
    memcpy(MAC_Origen, nic.ifr_hwaddr.sa_data, 6);
    printf("Direccion MAC de origen: ");
    displayMac(MAC_Origen, 6);
    printf("\n");

    if(ioctl(socket, SIOCGIFADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la direcion IP de origen\n");
        exit(-1);
    }

    memcpy(IP_ADDR, nic.ifr_addr.sa_data+2, 4);
    printf("Direccion IP de origen: ");
    displayDir(IP_ADDR, 4);
    printf("\n");

    if(ioctl(socket, SIOCGIFNETMASK, &nic) == -1){
        perror("Ha ocurrido un error al obtener la mascara de red\n");
        exit(-1);
    }

    memcpy(NET_MASK, nic.ifr_netmask.sa_data+2, 4);
    printf("Mascara de red: ");
    displayDir(NET_MASK, 4);
    printf("\n");

}

void setFrame(unsigned char *frame){
    memcpy(frame+0, MAC_Broad, 6); //MAC destino - Broadcast
    memcpy(frame+6, IP_ADDR, 6); //MAC origen
    memcpy(frame+12, ETH_TYPE, 2); //Tipo de trama
    memcpy(frame+14, "Rodrigo Alvarez Perez", 30);    
}

void sendFrame(int socket, unsigned char *frame){
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));

    interfaz.sll_family = AF_PACKET;
    interfaz.sll_protocol = htons(ETH_P_ALL);
    interfaz.sll_ifindex = interface_index;
    int tam = sendto(socket, frame, 60, 0, (struct sockaddr *) &interfaz, sizeof(interfaz));
    if(tam == - 1){
        perror("Ha ocurrido un error al enviar el paquete\n");
        exit(-1);
    }

    printf("Se ha enviado correctamente el paquete\n");

}

void displayFrame(unsigned char *frame, int tam){
    for(int i = 0; i < tam; i++){
        if(i % 16 == 0){
            printf("\n");
        }
        printf("%.2x ", frame[i]);
    }
    printf("\n");
}

void receiveFrame(int socket, unsigned char *frame){
    int tam = recvfrom(socket, frame, 1514, 0, NULL, 0);
    if(tam != -1){
        displayFrame(frame, tam);
    }else {
        perror("Ha ocurrido un error al obtener los datos\n");
        exit(-1);
    }
}

void displayMac(unsigned char MAC[6], int size){
    for(int i = 0; i < size; i++){
        if(i == 0){
            printf("%.2x", MAC[i]);
        }else {
            printf(":%.2x", MAC[i]);
        }
    }
    //printf("\n");
}

void displayDir(unsigned char DIR[4], int size){
    for(int i = 0; i< 4; i++){
        if(i==0){
            printf("%i", DIR[i]);
        }else{
            printf(".%i", DIR[i]);
        }
    }
    //printf("\n");
}
