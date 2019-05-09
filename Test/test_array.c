/*
 * c语言中多维数组的本质还是一维数组(array/1);
 * 可以使用指向数组的指针数组模拟真正的多维数组(foo/0)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int array(long (*arr)[4])
{
        printf("%d\n", sizeof arr);
        printf("%d\n", arr[0]);
        printf("%d\n", arr[1]);
        printf("%d\n", arr[2]);
        printf("%d\n", arr[3]);

        return 0;
}

int bar()
{
        printf("---------------------\n");
}

int foo()
{
        int (*arr[4])[4];
        int a0[4] = { 0,  1,  2,  3};
        int a1[4] = {10, 11, 12, 13};
        int a2[4] = {20, 21, 22, 23};
        int a3[4] = {30, 31, 32, 33};

        arr[0] = &a0;
        arr[1] = &a1;
        arr[2] = &a2;
        arr[3] = &a3;

        printf("%d\n", (*(arr[0]))[0]);
        printf("%d\n", (*(arr[1]))[0]);
        printf("%d\n", (*(arr[2]))[2]);

}

int main()
{
        long arr[4][4] = {};
        printf("%d\n", sizeof arr);
        array(arr);

        bar();
        foo();
        return 0;
}
