#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include "ww.h"

int isDirectory(char *name){
    struct stat data;

    int error = stat(name, &data);
    
    //check for errors
    if (error){
        perror(name);
        return 0;
    }
    //check if argv is a directory
    if (S_ISDIR(data.st_mode)){
        return 1;
    }
    return 0;

}

int main(int argc, char* argv[]){

    if(argc!=2)
        return EXIT_FAILURE;

    //if argv is a directory, word wrap all the files in it
    //TODO: change argv[1] -> argv[2]
    //TODO: check if this works with paths
   if (isDirectory(argv[1])) {
       puts("dir");
       //Now we find all the files besides "."/".."
       DIR *dirp = (opendir(argv[1]));
      // struct dirent *de;
       //TODO: replace d_type with the use of stat()
      /* 
        while ((de = readdir(dirp)) != NULL) {
            if (de->d_type == DT_REG) {
                printf("%s\n", de->d_name);
           }
       }
    */
       closedir(dirp);
   }
   
   return EXIT_SUCCESS;
}
