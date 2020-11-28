#include <iostream>
#include "mem.h"
using namespace std;

int main()
{
    int heap_size;
    //cout << "pointer size: " << sizeof(head) << " sizeof int: " << sizeof(int) << endl;
    printf("Enter heap size: ");
    scanf("%d", &heap_size);
    remove("output.txt");
    freopen("output.txt", "a", stdout);
    if (Mem_Init(heap_size) < 0)
    {
        printf("Error while allocating heap.\n");
        return 1;
    }
    else
    {
        int *p = (int *)Mem_Alloc(sizeof(int) * 30);
        char* p1 = (char *)Mem_Alloc(sizeof(char) * 50);
        float *p2 = (float *)Mem_Alloc(sizeof(float) * 30);
        display();
        Mem_Free(p+10, PARTIAL);
        display();
        Mem_Free(p1+40, FULL);
        display();
        cout << Mem_IsValid(p + 11) << endl;
        cout << Mem_GetSize(p) << endl;
        cout << Mem_GetSize(p2) << endl;
        cout << Mem_GetSize(p2 + 31) << endl;
        Mem_Clear();
        display();
    }
    return 0;
}
