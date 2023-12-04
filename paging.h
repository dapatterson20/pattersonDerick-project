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
int logicalAddr;
float numFault;
float numTLBHit;
float total;