#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>

unsigned char 
    ORIGIN_MAC[6], DEST_MAC[6], DEST_MAC_PING[6],
    ORIGIN_IP[4], NET_MASK[6],
    IP_ROUTER[4], MAC_BROAD[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    arpRequest[1514], arpResponse[1514], HW_TYPE[2] = { 0x00, 0x01 },
    ARP_ETH_TYPE[2] = { 0x08, 0x06 }, ETHERNET_PROTOCOL_TYPE[2] = {0x08, 0x00},
    ETHERNET_PROTOCOL_LEN[1] = {0x04},
    HW_LEN[1] = {0x06}, ARP_SEND_T[2] = { 0x00, 0x01 }, ARP_RECV_T[2] = { 0x00, 0x02 };
int INTERFACE_IDX, isInSameNet = 1;

void parseIpData(int *, char *);
void setHostData(int);
void checkDestIp(int *);
void displayMacAddr(char *);
void displayIPAddr(char *);
void setArpRequestData(char *);
void sendArpRequest(int);

void parseIpData(int *octets, char *ipDir){
    char *token;
    int cont = 0;
    token = strtok(ipDir, ".");
    while (token != NULL){
        int temp = atoi(token);
        octets[cont] = temp;
        token = strtok(NULL, ".");
        cont++;
    }
}

void setHostData(int socket){
    struct ifreq nic;
    char interface[20];
    printf("Inserta el nombre de la interfaz de red a ocupar: ");
    scanf("%s", interface);
    strcpy(nic.ifr_name, interface);
    if(ioctl(socket, SIOCGIFINDEX, &nic) == -1){
        perror("Ha ocurrido un error obtener el indice de la interfaz\n");
        exit(-1);
    }
    INTERFACE_IDX = nic.ifr_ifindex;

    if(ioctl(socket, SIOCGIFHWADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la mac de origen IP\n");
        exit(-1);
    }
    memcpy(ORIGIN_MAC, nic.ifr_hwaddr.sa_data, 6);

    if(ioctl(socket, SIOCGIFADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la direcion IP de origen\n");
        exit(-1);
    }

    memcpy(ORIGIN_IP, nic.ifr_addr.sa_data+2, 4);

    if(ioctl(socket, SIOCGIFNETMASK, &nic) == -1){
        perror("Ha ocurrido un error al obtener la mascara de red\n");
        exit(-1);
    }

    memcpy(NET_MASK, nic.ifr_netmask.sa_data+2, 4);

    if(ioctl(socket, SIOCGIFBRDADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la direccion IP de broadcast\n");
        exit(-1);
    }

    memcpy(IP_ROUTER, nic.ifr_broadaddr.sa_data+2, 4);
}

void checkDestIp(int *octects){
    for(int i = 0; i < 4; i++){
        int MASK_BYTE = NET_MASK[i];
        if((ORIGIN_IP[i] & MASK_BYTE) != (octects[i] & MASK_BYTE)){
            isInSameNet = 0;
            break;
        }
    }
}

void setArpRequestData(char *IP){
    memcpy(arpRequest+0, MAC_BROAD, 6); //MAC destino - Broadcast
    memcpy(arpRequest+6, ORIGIN_IP, 6); //IP origen
    memcpy(arpRequest+12, ARP_ETH_TYPE, 2); //Tipo de trama
    memcpy(arpRequest+14, HW_TYPE, 2); //Tipo de HW
    memcpy(arpRequest+16, ETHERNET_PROTOCOL_TYPE, 2); //Tipo de Protocolo
    memcpy(arpRequest+18, HW_LEN, 1); //Tamanio de Hw
    memcpy(arpRequest+19, ETHERNET_PROTOCOL_LEN, 1); //Tamanio de Protocolo
    memcpy(arpRequest+20, ARP_SEND_T, 2); //Tipo de Operacion
    memcpy(arpRequest+22, ORIGIN_MAC, 6); //MAC origen
    memcpy(arpRequest+28, ORIGIN_IP, 4); //IP Origen
    memcpy(arpRequest+32, MAC_BROAD, 6); //MAC Destino - Broadcast
    memcpy(arpRequest+38, IP, 4); //IP a preguntar MAC
}

void sendArpRequest(int socket){
    struct sockaddr_ll interfaz;
    memset(&interfaz, 0x00, sizeof(interfaz));

    interfaz.sll_family = AF_PACKET;
    interfaz.sll_protocol = htons(ETH_P_ALL);
    interfaz.sll_ifindex = interface_index;
    int tam = sendto(socket, arpRequest, 60, 0, (struct sockaddr *) &interfaz, sizeof(interfaz));
    if(tam == - 1){
        perror("Ha ocurrido un error al enviar el paquete\n");
        exit(-1);
    }

    printf("Peticion ARP enviada...\n");

    while(1){
        int tam = recvfrom(socket, arpRequest, 1514, 0, NULL, 0);
        if(tam != -1){
            
        }
    }

}