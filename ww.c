#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "ww.h"

int isDirectory(char *name){
    struct stat data;

    int error = stat(name, &data);
    
    if (error){
        perror(name);
        return -1;
    }
    if (S_ISDIR(data.st_mode)){
        return 1;
    }
    return 0;

}

int main(int argc, char* argv[])
{
   if (isDirectory(argv[1])){
       printf("%s is Directory\n", argv[1]);
   }
   return EXIT_SUCCESS;
}
