#include "syscall.h"

int main()
{
    int array[100];
    int size = 0, i = 0, j = 0;
    int swapped = 1,temp=0;
    PrintString("Nhap so phan tu cua mang: ");
    size = ReadInt();
    PrintChar('\n');
    for (i=0; i < size; i++)
    {
        array[i] = ReadInt();
    }
    while (swapped == 1)
    {
        swapped = 0;
        j++;
        for (i = 0; i < size - j; i++)
        {
            if(array[i]>array[i+1]){
                temp= array[i];
                array[i]=array[i+1];
                array[i+1]=temp;
                swapped=1;
            }
        }
    }
    PrintChar('\n');
    for(i=0;i<size;i++){
        PrintInt(array[i]);
        PrintChar(' ');
    }
    PrintChar('\n');
    return 0;
}
