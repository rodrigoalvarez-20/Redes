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
#include <sys/time.h>

void displayFrame(unsigned char *, int);
void displayMac(unsigned char[], int);
void displayDir(unsigned char[], int);

typedef struct Node {
    unsigned char IP_ADDR[4];
    unsigned char MAC_ADDR[6];
    struct Node *next;
}Nodo;

typedef struct ARPTable {
    Nodo *table;
}ArpTable;

void initTable(ArpTable *t){
    if(t == NULL){
        printf("La tabla es nula\n");
        exit(-1);
    }
    t->table = NULL;
}

int insertNode(ArpTable *t, char *ip, char *mac){
    if(t == NULL){
        printf("La tabla es nula\n");
        exit(-1);
    }

    Nodo *toAdd = (Nodo *) malloc(sizeof(Nodo));
    if(toAdd == NULL){
        printf("El nodo es nulo\n");
        return -1;
    }
    memcpy(toAdd->IP_ADDR, ip,4);
    memcpy(toAdd->MAC_ADDR, mac,6);
    toAdd->next = NULL;
    if(t->table == NULL){
        t->table = toAdd;
    }else {
        Nodo *last = t->table;
        while(last->next != NULL){
            last = last->next;
        }

        last->next = toAdd;
    }

    return 1;
}

void displayTable(ArpTable *t){
    if(t == NULL){
        printf("La tabla es nula\n");
        exit(-1);
    }

    printf("Tabla ARP\n");
    Nodo *init = t->table;
    printf("IP\t\t\tMAC\n");
    while(init != NULL){
        displayDir(init->IP_ADDR, 4);
        printf("\t\t");
        displayMac(init->MAC_ADDR, 6);
        printf("\n");
        init = init->next;
    }

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

void displayMac(unsigned char MAC[6], int size){
    for(int i = 0; i < size; i++){
        if(i == 0){
            printf("%.2x", MAC[i]);
        }else {
            printf(":%.2x", MAC[i]);
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
    //printf("\n");
}
