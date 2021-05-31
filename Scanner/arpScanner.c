#include "ARPTable.h"

unsigned char 
    MAC_Origen[6], 
    NET_MASK[4], 
    IP_ADDR[4],
    frameToSend[1514],
    rFrame[1514],
    ETH_TYPE[2] = {0x08, 0x06},
    MAC_Broad[6] = {0xff, 0xff,0xff,0xff,0xff,0xff},
    FRAME_SEND[2] = {0x00, 0x01},
    FRAME_RECV[2] = {0x00, 0x02},
    P_TYPE[2] = {0x08, 0x00},
    HW_LEN[1] = {0x06},
    P_LEN[1] = {0x04},
    HW_TYPE[2] = {0x00, 0x01};
int interface_index;
ArpTable tb;


void getData(int);
void displayMac(unsigned char[], int);
void displayDir(unsigned char[], int);
void setFrame(unsigned char *, int);
void sendFrame(int, unsigned char *);
void receiveFrame(int, unsigned char*);
void displayFrame(unsigned char *, int);

//Solicitud 00 01
//Respuesta 00 02
//21,22
//Ethertype 0800
//Protocolo 0604

int main(){
    int packet_socket;

    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if((packet_socket == -1)){
        perror("Ha ocurrido un error al crear el socket\n");
        exit(-1);
    }

    initTable(&tb);

    getData(packet_socket);
    for(int i = 1; i <= 254; i++){
        setFrame(frameToSend, i);
        sendFrame(packet_socket, frameToSend);
        receiveFrame(packet_socket, rFrame);    
    }
    displayTable(&tb);

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
    //printf("Direccion MAC de origen: ");
    //displayMac(MAC_Origen, 6);

    if(ioctl(socket, SIOCGIFADDR, &nic) == -1){
        perror("Ha ocurrido un error al obtener la direcion IP de origen\n");
        exit(-1);
    }

    memcpy(IP_ADDR, nic.ifr_addr.sa_data+2, 4);
    //printf("Direccion IP de origen: ");
    //displayDir(IP_ADDR, 4);

    if(ioctl(socket, SIOCGIFNETMASK, &nic) == -1){
        perror("Ha ocurrido un error al obtener la mascara de red\n");
        exit(-1);
    }

    memcpy(NET_MASK, nic.ifr_netmask.sa_data+2, 4);
    //printf("Mascara de red: ");
    //displayDir(NET_MASK, 4);

}

void setFrame(unsigned char *frame, int lastOct){
    memcpy(frame+0, MAC_Broad, 6); //MAC destino - Broadcast
    memcpy(frame+6, IP_ADDR, 6); //MAC origen
    memcpy(frame+12, ETH_TYPE, 2); //Tipo de trama
    memcpy(frame+14, HW_TYPE, 2); //Tipo de HW
    memcpy(frame+16, P_TYPE, 2); //Tipo de Protocolo
    memcpy(frame+18, HW_LEN, 1); //Tamanio de Hw
    memcpy(frame+19, P_LEN, 1); //Tamanio de Protocolo
    memcpy(frame+20, FRAME_SEND, 2); //Tipo de Operacion
    memcpy(frame+22, MAC_Origen, 6); //MAC origen
    memcpy(frame+28, IP_ADDR, 4); //IP Origen
    memcpy(frame+32, MAC_Broad, 6); //MAC Destino - Broadcast
    //printf("%s\n", IP_ADDR & MAC_Origen);
    unsigned char IP_DST[4] = { 192,168,100,lastOct };
    displayDir(IP_DST, 4);
    printf("\n");
    memcpy(frame+38, IP_DST, 4); //IP a descubrir
    
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

void receiveFrame(int socket, unsigned char *frame){
    struct timeval start, end;
    long mtime = 0, sec, usec;
    gettimeofday(&start, NULL);
    int flag = 0;
    while(mtime < 200){
        int tam = recvfrom(socket, frame, 1514, MSG_DONTWAIT, NULL, 0);
        if(tam != -1){
            if( !memcmp(frame, MAC_Origen, 6) && !memcmp(frame+20, FRAME_RECV, 2)){
                unsigned char ip[4], mac[6];
                memcpy(mac, frame+22, 6);
                memcpy(ip, frame+28, 4);
                if(insertNode(&tb, ip, mac)){
                    printf("La IP ");
                    displayDir(ip, 4);
                    printf(" se ha agregado a la tabla ARP\n");
                    flag = 1;
                }else {
                    exit(-1);
                }
            }
        }
        gettimeofday(&end, NULL);
        sec = end.tv_sec - start.tv_sec;
        usec = end.tv_usec - start.tv_usec;
        mtime = ( (sec) * 1000 + usec/1000.0) + 0.5;
        if(flag == 1){
            break;
        }
    }
}
