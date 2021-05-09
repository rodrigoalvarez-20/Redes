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
    ETH_TYPE[2] = {0x0c, 0x0c},
    frameToSend[1514],
    MAC_Broad[6] = {0xff, 0xff,0xff,0xff,0xff,0xff};
int interface_index;


void getData(int);
void displayMac(unsigned char[], int);
void displayDir(unsigned char[], int);
void setFrame(unsigned char *);
void sendFrame(int, unsigned char *);

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

    if(ioctl(socket, SIOCGIFADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la direcion IP de origen\n");
        exit(-1);
    }

    memcpy(IP_ADDR, nic.ifr_addr.sa_data+2, 4);
    printf("Direccion IP de origen: ");
    displayDir(IP_ADDR, 4);

    if(ioctl(socket, SIOCGIFNETMASK, &nic) == -1){
        perror("Ha ocurrido un error al obtener la mascara de red\n");
        exit(-1);
    }

    memcpy(NET_MASK, nic.ifr_netmask.sa_data+2, 4);
    printf("Mascara de red: ");
    displayDir(NET_MASK, 4);

}

void setFrame(unsigned char *frame){
    memcpy(frame+0, MAC_Broad, 6);
    memcpy(frame+6, IP_ADDR, 6);
    memcpy(frame+12, ETH_TYPE, 2);
    memcpy(frame+14, "Rodrigo Alvarez Perez", 22);
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

void displayMac(unsigned char MAC[6], int size){
    for(int i = 0; i < size; i++){
        if(i == 0){
            printf("%.2x", MAC_Origen[i]);
        }else {
            printf(":%.2x", MAC_Origen[i]);
        }
    }
    printf("\n");
}

void displayDir(unsigned char DIR[4], int size){
    for(int i = 0; i< 4; i++){
        if(i==0){
            printf("%i", DIR[i]);
        }else{
            printf(".%i", DIR[i]);
        }
    }
    printf("\n");
}
