#include "frames.h"

int main(){
    FILE *f;
    char fileName[20], *line = NULL;
    size_t len = 0;
    ssize_t read;
    int idxSel, opcSel;
    Queue q;
    initQueue(&q);
    printf("Analizador de tramas LLC\n");
    printf("Introduce el nombre del archivo: ");
    scanf("%s", fileName);
    f = fopen(fileName, "r");
    fflush(stdin);
    while((read = getline(&line, &len, f)) != -1){
        if(read != 1){
            addToQueue(&q, line, read);
        }
    }
    free(line);
    printf("Mostrando datos: \n");
    showQueue(&q);
    printf("\n");
    
    do {
        printf("\n");
        printf("Introduce el indice del elemento a evaluar: ");
        scanf("%d", &idxSel);
        analyzeFrame(getByIndex(&q, idxSel));
        printf("\n");
        printf("Desea continuar? [1]Si. [2]No. ");
        
        scanf("%d", &opcSel);
    }while(opcSel != 2);
    
    printf("Hasta luego\n");

    return 0;
}
