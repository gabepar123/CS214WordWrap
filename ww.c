#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ww.h"


int printTest(int x){

    printf("Test: %d\n", x);

}

int main(int argc, char* argv[])
{
    printTest(atoi(argv[1]));
    //test2
}
