#include <iostream>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstdio>
#include "mem.h"
using namespace std;

bool callTOmmap = true;
int AllocCount;
int FreeCount;
int userArea;

metadata *freeHead, *freeTail, *allocHead, *allocTail;
void *ptr = NULL;
const int page_size = getpagesize();
const int metadata_size = sizeof(metadata);
int total_free;

void Mem_Clear()
{
    char *temp = (char *)ptr;
    allocHead = (metadata *)(temp + userArea);
    allocHead->size = 0;
    allocHead->baseAddr = NULL;
    allocTail = allocHead;
    freeHead = (metadata *)(temp + userArea - metadata_size);
    freeHead->size = userArea;
    freeHead->baseAddr = ptr;
    freeTail = freeHead;
    total_free = userArea;
    FreeCount = 1;
    AllocCount = 0;
}

int Mem_Init(int size_of_region)
{
    if (size_of_region <= 0)
    {
        cout << "Invalid size, size should be greater than zero" << endl;
        return -1;
    }
    if (callTOmmap)
    {
        callTOmmap = false;
        userArea = size_of_region;
        printf("page size: %d\n", page_size);
        total_free = size_of_region;
        size_of_region *= PS_MULTIPLIER;
        if (size_of_region % page_size != 0)
        {
            size_of_region = (size_of_region / page_size + 1) * page_size;
        }
        int fd = open("/dev/zero", O_RDWR);
        ptr = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
        if (ptr == MAP_FAILED)
        {
            perror("mmap error");
            return -1;
        }
        if (close(fd) < 0)
        {
            perror("Error closing file.\n");
            return -1;
        }
        printf("Allocated %d bytes at %p addr\n", size_of_region, ptr);
        char *temp = (char *)ptr;
        Mem_Clear();
        return 0;
    }
    cout << "your memory allocator must call mmap only one time (when it is first initialized)" << endl;
    return -1;
}

void *Mem_Alloc(int size)
{
    if (size == 0)
    {
        printf("0 bytes allocated");
        return NULL;
    }
    if (size < 0)
    {
        printf("size can't be negative");
        return NULL;
    }
    if (size > total_free)
    {
        printf("Not enough free memory available.");
        return NULL;
    }
    total_free -= size;
    metadata *temp = freeHead;
    bool f = 0;
    for (int i = 0; i < FreeCount; i++)
    {
        if (temp->size >= size)
        {
            f = 1;
            break;
        }
        // --temp;
    }
    if (!f)
    {
        cout << "external Fragmentation" << endl;
        return NULL;
    }
    metadata *newHead;
    if (!allocHead->baseAddr)
    {
        newHead = allocHead;
    }
    else
    {
        newHead = allocTail + 1;
    }
    allocTail = newHead;
    AllocCount++;
    newHead->size = size;
    newHead->baseAddr = temp->baseAddr;
    if (temp->size > size)
    {
        temp->size -= size;
        char *temp1 = (char *)temp->baseAddr;
        temp1 += size;
        temp->baseAddr = temp1;
    }
    else
    {
        FreeCount--;
        temp->size = freeTail->size;
        temp->baseAddr = freeTail->baseAddr;
        freeTail += 1;
    }
    return allocTail->baseAddr;
}

void display()
{
    cout << "Total free: " << total_free << " FreeCount: " << FreeCount << " AllocCount: " << AllocCount << endl;
    cout << "Free List:\n";
    metadata *temp = freeHead;
    for (int i = 0; i < FreeCount; ++i)
    {
        cout << temp->baseAddr << "\t" << temp->size << endl;
        --temp;
    }
    cout << "Alloc List:\n";
    if (!AllocCount)
        cout << "No process allocated" << endl;
    temp = allocHead;
    for (int i = 0; i < AllocCount; ++i)
    {
        cout << temp->baseAddr << "\t" << temp->size << endl;
        ++temp;
    }
}

void *Mem_IsValid(void *ptr)
{
    char *p = (char *)ptr;
    metadata *temp = allocHead;
    int i;
    for (i = 0; i < AllocCount; i++)
    {

        if ((char *)temp->baseAddr <= p && (char *)temp->baseAddr + temp->size > p)
        {
            return temp;
        }
        temp = temp + 1;
    }
    cout << "Invalid pointer\n";
    return NULL;
}

int Mem_GetSize(void *t)
{
    metadata *t1;
    t1 = (metadata *)Mem_IsValid(t);
    //char *t1 = (char *);
    if (t1)
    {
        return t1->size;
    }
    else
    {
        return -1;
    }
}

int Mem_Free(void *freePtr, bool flag = FULL)
{
    if (freePtr == NULL)
    {
        cout << "Unable to Free Null ptr." << endl;
        return 0;
    }
    metadata *node = (metadata *)Mem_IsValid(freePtr);
    if (!node)
    {
        cout << "Process Image doesn't exists" << endl;
        return -1;
    }
    metadata *freenode = freeTail - 1;
    freenode->baseAddr = node->baseAddr;
    freenode->size = node->size;
    cout << "FreeNODE SIZE" << freenode->size << endl;
    freeTail--;
    FreeCount++;
    node->size = (char *)freePtr - (char *)node->baseAddr;
    if (node->size == 0 || flag == FULL)
    {
        node->size = allocTail->size;
        node->baseAddr = allocTail->baseAddr;
        allocTail--;
        AllocCount--;
    }
    else
    {
        freenode->baseAddr = (void *)((char *)node->baseAddr + node->size);
        freenode->size -= node->size;
    }
    total_free += freenode->size;
    merge_left(freeTail);
    merge_right(freeTail);
    return 0;
}

void merge_right(metadata *temp)
{

    char *base = (char *)temp->baseAddr;
    char *base1;
    base = base + temp->size;
    metadata *current = freeHead;
    for (int i = 1; i <= FreeCount; i++)
    {
        base1 = (char *)current->baseAddr;
        if (base == base1)
        {
            temp->size = temp->size + current->size;
            current->size = temp->size;
            current->baseAddr = temp->baseAddr;
            freeTail = freeTail + 1;
            FreeCount--;
            break;
        }
        else
        {
            current = current - 1;
        }
    }
}

void merge_left(metadata *temp)
{
    char *base = (char *)temp->baseAddr;
    char *base1;

    metadata *current = freeHead;
    for (int i = 1; i <= FreeCount; i++)
    {
        base1 = (char *)current->baseAddr + current->size;
        if (base1 == base)
        {
            current->size = current->size + temp->size;
            freeTail = freeTail + 1;
            FreeCount--;
            break;
        }
        else
        {
            current = current - 1;
        }
    }
}