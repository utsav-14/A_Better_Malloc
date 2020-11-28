#ifndef MEM
#define MEM
#define PS_MULTIPLIER 4
#define PARTIAL true
#define FULL false
typedef struct metadata
{
    int size;
    void *baseAddr;
}metadata;

int Mem_Init(int);
void *Mem_Alloc(int);
int Mem_Free(void *, bool);
void *Mem_IsValid(void *);
int Mem_GetSize(void *);
void display();
void Mem_Clear();
void merge_left(metadata *);
void merge_right(metadata *);
#endif