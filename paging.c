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

translationLookasideBuffer tlb;
pageTable pTable;

char* binary;
int memory[256][256];
int memIndex;
int tlbIndex;

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
    binary=malloc(65536);
    for (int x=0; x<65536; x++) {
        fread(&binary[x], 65537,1, fp);
        //printf("Binary: %d, %d\n", binary[x], x);
    }
    printf("%d\n",binary[64243]);

}

int findFrame(int pageNum, int offset) {
    int foundIndex=0;
    bool inTLB;
    for (int x=0; x<16; x++) {
        if (tlb.pageNumber[x]==pageNum) {
            inTLB=true;
            foundIndex=x;
            break;
        }
    }
    printf("a\n");
    //TLB miss
    if (!inTLB) {
        printf("b\n");
        printf("%d\n",pTable.validBit[pageNum]);
        if (pTable.validBit[pageNum]==0) {
            printf("c\n");
            //Page fault; consult backing store.
            int start=pageNum*256;
            for (int i=0; i<256; i++) {
                memory[memIndex][i]=binary[start+i];
                //printf("%d,%d,%d\n",memory[memIndex][i],binary[start+i],i);
            }
            
            //Update tlb
            tlb.pageNumber[tlbIndex]=pageNum;
            tlb.frame[tlbIndex]=memIndex;
            tlbIndex++;
            if (tlbIndex>15) {
                tlbIndex=0;
            }

            //Update Page table
            pTable.frame[pageNum]=memIndex;
            pTable.validBit[pageNum]=1;

            //Finally, access memory from frame & offset
            printf("Do it: %d\n",memory[memIndex][offset]);
            memIndex++;
            return memory[memIndex][offset];
        }
        else {
            //Update tlb
            tlb.pageNumber[tlbIndex]=pageNum;
            tlb.frame[tlbIndex]=memIndex;
            tlbIndex++;
            if (tlbIndex>15) {
                tlbIndex=0;
            }

            //Found frame in page table
            printf("Oh yeah do it: %d\n",memory[pTable.frame[pageNum]][offset]);
            return memory[pTable.frame[pageNum]][offset];
        }
    }
    //Page number found.
    else {
        /*
        for (int j=0; j<256; j++) {
            printf("%d, %d\n",memory[tlb.pageNumber[foundIndex]][j],j);
        }
        */
        printf("%d\n",tlb.pageNumber[foundIndex]);
        printf("%d\n",offset);
        printf("Yeah do it: %d\n",memory[tlb.frame[foundIndex]][offset]);
        return memory[tlb.frame[foundIndex]][offset];
    }
    return 0;
}

int main() {
    for (int x=0; x<256; x++) {
        pTable.validBit[x]=0;
    }
    read_from_file("addresses.txt");
    readBinary("BACKING_STORE.bin");
    findFrame(250, 243);
    /*
    for (int y=0; y<256; y++) {
        printf("m %d\n",memory[0][y]);
    }
    */
    findFrame(250, 242);
    return 0;
}