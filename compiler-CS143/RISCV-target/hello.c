#include <stdio.h>
#include <stdlib.h>

int mul(int c){
    return c * c;
}

int main()
{
    mul(mul(5));
}