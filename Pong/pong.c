#include "util.h"

int main(int argc, char **argv){
    char *ipDir = NULL;
    int noSol = 0, ipOctets[4], packet_socket;
    printf("Bienvenido a la utilidad Pong %d\n", argc);
    if(argc < 3) {
        ipDir = (char *) malloc(15*sizeof(char));
        printf("Introduzca el numero de solicitudes a enviar: ");
        scanf("%d", &noSol);    
        printf("Por favor introduzca la direccion ip de destino: ");
        scanf("%s", ipDir);
    }else {
        ipDir = argv[2];
        noSol = *argv[1] - '0';
    }
    parseIpData(ipOctets, ipDir);
    packet_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));  
    if(!socket){
        perror("Ha ocurrido un error al crear el socket\n");
        exit(-1);
    }

    setHostData(packet_socket);
    checkDestIp(ipOctets);
    
    if(isInSameNet){
        char IP_Str[4] = { ipOctets[0], ipOctets[1], ipOctets[2], ipOctets[3] };
        setArpRequestData(IP_Str);
    }else{
        setArpRequestData(IP_ROUTER);
    }

    //Enviar solicitud y esperar datos
    sendArpRequest(packet_socket);

    return 0;
}