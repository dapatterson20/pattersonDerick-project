#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct pageTable {
    int frame[256];
    int validBit[256];
}pageTable;

typedef struct translationLookasideBuffer {
    int pageNumber[16];
    int frame[16];
}translationLookasideBuffer;

void read_from_file(char* filename){
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    int* page = NULL;

    page=(int*)malloc(sizeof(int*)*15);

    int pageNumber;
    int offset;

    for (int i=0; i<15; i++) {
        fscanf(fp, "%d\n", &page[i]);
        pageNumber=(page[i]>>8)&0x00FF;
        offset=page[i]&0x00FF;
        printf("%d\n",page[i]);
        printf("%d\n",pageNumber);
        printf("%d\n",offset);
    }
}

void readBinary(char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    char* binary=malloc(256);
    for (int x=0; x<256; x++) {
        fread(&binary[x], 257,1, fp);
        printf("Binary: %d, %d\n", binary[x], x);
    }

}

void findFrame(int pageNum, int offset) {
    translationLookasideBuffer tlb;
    pageTable pTable;
    bool inTLB;
    for (int x=0; x<16; x++) {
        if (tlb.pageNumber[x]==pageNum) {
            inTLB=true;
            break;
        }
    }
    if (!inTLB) {
        if (pTable.validBit[pageNum]==0) {
            //Page fault; consult backing store.
        }
    }
}

int main() {
    read_from_file("addresses.txt");
    readBinary("BACKING_STORE.bin");
    return 0;
}