#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct frame {
    int index;
    char *hexData;
    struct frame *nextFrame;
} Frame;

typedef struct queue {
    Frame *root;
    int size;
} Queue;

void initQueue(Queue *);
int addToQueue(Queue *, char *, int);
Frame getByIndex(Queue *, int);
void analyzeFrame(Frame);
void displayFrameData(Frame);
int hexToInt(char *);
void getSAPType(char *);
void getDSAPType(char *);
void getSSAPType(char *);
void getBinaryFromString(char *, char *);
void getBinaryFromChar(char, char *);
int binaryToDecimal(char *);
int getFrameType(char *);
void getNNName(char *);

void initQueue(Queue *q){
    if(q == NULL){
        printf("La cola es nula\n");
        exit(-1);
    }
    q->root = NULL;
    q->size = 0;
}

int addToQueue(Queue *q, char *data, int len){
    if(q == NULL){
        printf("La cola es nula\n");
        exit(-1);
    }
    Frame *frmToAdd = (Frame *)malloc(sizeof(Frame));
    if(frmToAdd == NULL){
        printf("Ha ocurrido un error al crear el nodo\n");
        exit(-1);
    }
    
    frmToAdd->hexData = (char *)malloc(len);
    strcpy(frmToAdd->hexData, data);
    frmToAdd->index = q->size;
    frmToAdd->nextFrame = NULL;

    if(q->root == NULL){
        q->root = frmToAdd;
        q->size++;
    }else {
        Frame *actualFrm = q->root;
        while(actualFrm->nextFrame != NULL){
            actualFrm = actualFrm->nextFrame;
        }
        actualFrm->nextFrame = frmToAdd;
        q->size++;
    }
    return 1;
}

Frame getByIndex(Queue *q, int idx){
    if(q == NULL){
        printf("La cola es nula\n");
        exit(-1);
    }
    if(idx > q->size){
        printf("El indice no esta en rango\n");
        exit(-1);
    }
    for(Frame *actual = q->root; actual != NULL; actual = actual->nextFrame){
        if(actual->index == idx)
            return *actual;
    }
    
}

void analyzeFrame(Frame frmData){
    char longitud[] = { frmData.hexData[24], frmData.hexData[25], frmData.hexData[26], frmData.hexData[27] };
    char *DSAP = NULL, *SSAP = NULL, *control = NULL;
    int frameLen = hexToInt(longitud);
    printf("Trama:\n");
    displayFrameData(frmData);
    printf("\n");
    printf("Direccion MAC de destino: ");
    for(int i = 0; i < 12; i+=2){
        printf("%c%c ", frmData.hexData[i], frmData.hexData[i+1]);
    }
    printf("\n");
    printf("Direccion MAC de origen: ");
    for(int i = 0; i < 12; i+=2){
        printf("%c%c ", frmData.hexData[12+i], frmData.hexData[12+i+1]);
    }
    printf("\n");
    printf("Longitud: %d\n", frameLen);
    if(frameLen < 1500){
        //Trama LLC
        DSAP = (char *) malloc(2);
        SSAP = (char *) malloc(2);
        control = (char *) malloc(4);
        DSAP[0] = frmData.hexData[28];
        DSAP[1] = frmData.hexData[29];
        SSAP[0] = frmData.hexData[30];
        SSAP[1] = frmData.hexData[31];
        control[0] = frmData.hexData[32];
        control[1] = frmData.hexData[33];
        control[2] = frmData.hexData[34];
        control[3] = frmData.hexData[35];
        printf("DSAP: %s - ", DSAP);
        getSAPType(DSAP);
        printf(" - ");
        getDSAPType(DSAP);
        printf("\n");
        printf("SSAP: %s - ", SSAP );
        getSAPType(SSAP);
        printf(" - ");
        getSSAPType(SSAP);
        printf("\n");
        int frameType = getFrameType(control);
        printf("Tipo de trama: ");
        if(frameType == 1){
            printf("Informacion\n");
            char NSF[9], NRF[9], NS[8], NR[8];
            char *nsArray, *nrArray;
            nsArray = (char *) malloc(2);
            nrArray = (char *) malloc(2);
            strncpy(nsArray, control, 2);
            strncpy(nrArray, control+2, 2);
            getBinaryFromString(nsArray, NS);
            getBinaryFromString(nrArray, NR);
            strncpy(NSF, NS, 7);
            strncpy(NRF, NR, 7);
            
            printf("N(s): %d\n", binaryToDecimal(NSF));
            printf("N(r): %d\n", binaryToDecimal(NRF));
            
        }else if(frameType == 2){
            printf("Supervision\n");
            char NRF[9], NR[8], SSArr[5];
            char *nrArray, *SS;
            nrArray = (char *) malloc(2);
            SS = (char *) malloc(2);
            strncpy(nrArray, control+2, 2);
            getBinaryFromChar(control[1], SSArr);
            strncpy(SS, SSArr, 2);
            getBinaryFromString(nrArray, NR);
            strncpy(NRF, NR, 7);
            
            printf("SS: ");
            if(!strcmp(SS, "00"))
                printf("RR\n");
            else if(!strcmp(SS, "01"))
                printf("RNR\n");
            else if(!strcmp(SS, "10"))
                printf("REJ\n");
            else if(!strcmp(SS, "11"))
                printf("SREJ\n");

            printf("N(r): %d\n", binaryToDecimal(NRF));

        }else {
            printf("No numerada\n");
            char *MMArray, *MM, *MMF;
            MMArray =  (char *) malloc(2);
            MM = (char *) malloc(8);
            MMF = (char *) malloc(6);
            
            strncpy(MMArray, control, 2);
            
            getBinaryFromString(MMArray, MM);
            
            strncpy(MMF, MM, 6);
            printf("Bits MM %s\n", MMF);
            printf("Valor MM: ");
            getNNName(MMF);
            
        } 
    }else {
        printf("La trama no es LLC\n");
    }
}

void getNNName(char *value){
    if(!strcmp(value, "100100"))
        printf("SNRM");
    else if(!strcmp(value, "110111"))
        printf("SNRME");
    else if(!strcmp(value, "000111"))
        printf("SARM");
    else if(!strcmp(value, "010111"))
        printf("SARME");
    else if(!strcmp(value, "001111"))
        printf("SABM");
    else if(!strcmp(value, "011111"))
        printf("SABME");
    else if(!strcmp(value, "000101"))
        printf("SIM");
    else if(!strcmp(value, "010100"))
        printf("DISC");
    else if(!strcmp(value, "011000"))
        printf("UA");
    else if(!strcmp(value, "000011"))
        printf("DM");
    else if(!strcmp(value, "010000"))
        printf("RD");
    else if(!strcmp(value, "000001"))
        printf("RIM");
    else if(!strcmp(value, "000000") || !strcmp(value, "000100") )
        printf("UI");
    else if(!strcmp(value, "001100"))
        printf("UP");
    else if(!strcmp(value, "100111"))
        printf("RSET");
    else if(!strcmp(value, "101111") || !strcmp(value, "101011"))
        printf("XID");
    else if(!strcmp(value, "111100") || !strcmp(value, "111000"))
        printf("TEST");
}

int getFrameType(char *control){
    char type[5];
    getBinaryFromChar(control[1], type);
    if(type[strlen(type) -1] == '0'){
        //Informacion
        return 1;
    }else if(type[strlen(type) -1] == '1' && type[strlen(type) -2] == '0'){
        //Supervision
        return 2;
    }else if(type[strlen(type) -1] == '1' && type[strlen(type) -2] == '1'){
        //No numerada
        return 3;
    }
}

void getDSAPType(char *value){
    char binary[9];
    getBinaryFromString(value, binary);
    if(binary[strlen(binary) - 1] == '0')
        printf("Individual");
    else if(binary[strlen(binary) - 1] == '1')
        printf("Grupo");
}

void getSSAPType(char *value){
    char binary[9];
    getBinaryFromString(value, binary);
    if(binary[strlen(binary) - 1] == '0')
        printf("Comando");
    else if(binary[strlen(binary) - 1] == '1')
        printf("Respuesta");
}

void getBinaryFromString(char *string, char *binary){
    for(int i = 0; i < strlen(string); i++){
        switch (string[i]){
        case '0':
            strcat(binary, "0000");
            break;
        case '1':
            strcat(binary, "0001");
            break;
        case '2':
            strcat(binary, "0010");
            break;
        case '3':
            strcat(binary, "0011");
            break;
        case '4':
            strcat(binary, "0100");
            break;
        case '5':
            strcat(binary, "0101");
            break;
        case '6':
            strcat(binary, "0110");
            break;
        case '7':
            strcat(binary, "0111");
            break;
        case '8':
            strcat(binary, "1000");
            break;
        case '9':
            strcat(binary, "1001");
            break;
        case 'A':
        case 'a':
            strcat(binary, "1010");
            break;
        case 'B':
        case 'b':
            strcat(binary, "1011");
            break;
        case 'C':
        case 'c':
            strcat(binary, "1100");
            break;
        case 'D':
        case 'd':
            strcat(binary, "1101");
            break;
        case 'E':
        case 'e':
            strcat(binary, "1110");
            break;
        case 'F':
        case 'f':
            strcat(binary, "1111");
            break;
        }
    }



}

void getBinaryFromChar(char caracter, char *binary){
    switch (caracter){
        case '0':
            strcpy(binary, "0000");
            break;
        case '1':
            strcpy(binary, "0001");
            break;
        case '2':
            strcpy(binary, "0010");
            break;
        case '3':
            strcpy(binary, "0011");
            break;
        case '4':
            strcpy(binary, "0100");
            break;
        case '5':
            strcpy(binary, "0101");
            break;
        case '6':
            strcpy(binary, "0110");
            break;
        case '7':
            strcpy(binary, "0111");
            break;
        case '8':
            strcpy(binary, "1000");
            break;
        case '9':
            strcpy(binary, "1001");
            break;
        case 'A':
        case 'a':
            strcpy(binary, "1010");
            break;
        case 'B':
        case 'b':
            strcpy(binary, "1011");            
            break;
        case 'C':
        case 'c':
            strcpy(binary, "1100");
            break;
        case 'D':
        case 'd':
            strcpy(binary, "1101");
            break;
        case 'E':
        case 'e':
            strcpy(binary, "1110");
            break;
        case 'F':
        case 'f':
            strcpy(binary, "1111");
            break;
    }
}

int binaryToDecimal(char *binary){
    int acum = 0;
    int pot = 0;
    for(int i = strlen(binary) -1; i >= 0; i--){
        if(binary[i] == '1')
            acum += pow(2, pot);
        pot++;
    }
    return acum;
}

int hexToInt(char *hex){
    int decimal = 0;
    int base = 1;
    for(int i = strlen(hex)-1; i >= 0; i--){
        if(hex[i] >= '0' && hex[i] <= '9'){
            decimal += (hex[i] - 48) * base;
            base *= 16;
        }else if(hex[i] >= 'A' && hex[i] <= 'F'){
            decimal += (hex[i] - 55) * base;
            base *= 16;
        }else if(hex[i] >= 'a' && hex[i] <= 'f'){
            decimal += (hex[i] - 87) * base;
            base *= 16;
        }
    }

    return decimal;
}

void showQueue(Queue *q){
    if(q == NULL){
        printf("La cola es nula\n");
        exit(-1);
    }
    for(Frame *actual = q->root; actual != NULL; actual = actual->nextFrame){
        displayFrameData(*actual);
        printf("\n");
    }
}

void getSAPType(char *value){
    if(!strcmp(value,"00"))
        printf("Null SAP");
    if(!strcmp(value,"02"))
        printf("Individual LLC Sublayer Management Function");
    if(!strcmp(value,"03"))
        printf("Group LLC Sublayer Management Function");
    if(!strcmp(value,"04"))
        printf("IBM SNA Path Control (individual)");
    if(!strcmp(value,"05"))
        printf("IBM SNA Path Control (group)");
    if(!strcmp(value,"06"))
        printf("ARPANET Internet Protocol (IP)");
    if(!strcmp(value,"08") || !strcmp(value,"0C"))
        printf("SNA");
    if(!strcmp(value,"0E") || !strcmp(value,"0e"))
        printf("PROWAY (IEC955) Network Management & Initialization");
    if(!strcmp(value,"18"))
        printf("Texas Instruments");
    if(!strcmp(value,"42"))
        printf("IEEE 802.1 Bridge Spanning Tree Protocol");
    if(!strcmp(value,"4E") || !strcmp(value,"4e"))
        printf("EIA RS-511 Manufacturing Message Service");
    if(!strcmp(value,"7E") || !strcmp(value,"7e"))
        printf("ISO 8208");
    if(!strcmp(value,"80"))
        printf("Xerox Network Systems");
    if(!strcmp(value,"86"))
        printf("Nestar");
    if(!strcmp(value,"8E") || !strcmp(value,"8e"))
        printf("PROWAY (IEC 955) Active Station List Maintenance");
    if(!strcmp(value,"98"))
        printf("ARPANET Address Resolution Protocol");
    if(!strcmp(value,"BC") || !strcmp(value,"bc"))
        printf("Banyan VINES");
    if(!strcmp(value,"AA") || !strcmp(value,"aa"))
        printf("SubNetwork Access Protocol");
    if(!strcmp(value,"E0") || !strcmp(value,"e0"))
        printf("Novell NetWare");
    if(!strcmp(value,"F0") || !strcmp(value,"f0") || !strcmp(value,"f1") || !strcmp(value,"F1"))
        printf("IBM NetBIOS");
    if(!strcmp(value,"F4") || !strcmp(value,"f4"))
        printf("IBM LAN Management (individual)");
    if(!strcmp(value,"F5") || !strcmp(value,"f5"))
        printf("IBM LAN Management (group)");
    if(!strcmp(value,"F8") || !strcmp(value,"f8"))
        printf("IBM Remote Program Load (RPL)");
    if(!strcmp(value,"FA") || !strcmp(value,"fa"))
        printf("Ungermann-Bass");
    if(!strcmp(value,"FE") || !strcmp(value,"fe"))
        printf("ISO Network Layer Protocol");
    if(!strcmp(value,"FF") || !strcmp(value,"ff"))
        printf("Global LSAP");
}

void displayFrameData(Frame dt){
    printf("Q[%d]:\n", dt.index);
    for(int i = 0; i < strlen(dt.hexData); i+=2){
        if(i != 0 && i % 32 == 0){
            printf("\n");
        }
        printf("%c%c ", dt.hexData[i], dt.hexData[i+1]);
    }
}