#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include "paging.h"

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
    //printf("%d\n",binary[64243]);

}

int findFrame(int pageNum, int offset) {
    total+=1;
    int foundIndex=0;
    bool inTLB=false;
    for (int x=0; x<16; x++) {
        if (tlb.pageNumber[x]==pageNum) {
            inTLB=true;
            foundIndex=x;
            break;
        }
    }
    //printf("a\n");
    //TLB miss
    if (!inTLB) {
        //printf("b\n");
        //printf("%d\n",pTable.validBit[pageNum]);
        if (pTable.validBit[pageNum]==0) {
            //printf("c\n");
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
            printf("%d, %d, %d\n",logicalAddr, pageNum*256, memory[memIndex][offset]);
            memIndex++;
            numFault+=1;
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
            printf("%d, %d, %d\n",logicalAddr, pageNum*256, memory[pTable.frame[pageNum]][offset]);
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
        //printf("%d\n",tlb.pageNumber[foundIndex]);
        //printf("%d\n",offset);
        printf("%d, %d, %d\n",logicalAddr, pageNum*256, memory[tlb.frame[foundIndex]][offset]);
        numTLBHit+=1;
        return memory[tlb.frame[foundIndex]][offset];
    }
}

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

    for (int i=0; i<1000; i++) {
        fscanf(fp, "%d\n", &page[i]);
        pageNumber=(page[i]>>8)&0x00FF;
        offset=page[i]&0x00FF;
        //printf("%d\n",page[i]);
        //printf("%d\n",pageNumber);
        //printf("%d\n",offset);
        logicalAddr=page[i];
        findFrame(pageNumber, offset);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2){
        fprintf(stderr,"SYNOPSIS: %s <file name>\n",argv[0]);
        return 1;
    }

    for (int x=0; x<256; x++) {
        pTable.validBit[x]=0;
    }
    readBinary("BACKING_STORE.bin");
    read_from_file(argv[1]);
    printf("Page-fault rate- %f\n",numFault/total);
    printf("TLB hit rate- %f\n",numTLBHit/total);
    /*
    findFrame(255,255);
    findFrame(250, 243);
    findFrame(29,167);
    
    for (int y=0; y<256; y++) {
        printf("m %d\n",memory[0][y]);
    }
    
    findFrame(250, 242);
    findFrame(29,166);
    */
    return 0;
}