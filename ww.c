#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
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

int wrap(char * filename, int width){

    //TODO: figure out why bytes_read doesn't work.
    //TODO: actually format the file properly
    int bytes_read=0;
    char* buf = malloc(sizeof(char)*width);
    int f = open(filename, O_RDONLY);
    //if you can't open the file, return failure
    if(f==-1)
        return 1;

    bytes_read = read(f,&buf,width);

    close(f);
    free(buf);
    return 0;
}

int main(int argc, char* argv[]){

    //Check if proper arguments are given. If it is not 3 (ex: [./ww 80]), then return with EXIT_FAILURE.
    if(argc!=3)
        return EXIT_FAILURE;

    //if argv is a directory, word wrap all the files in it
    //TODO: change argv[1] -> argv[2] [FIXED by broteam/iman]
    //TODO: check if this works with paths
   if (isDirectory(argv[2])) {
       puts("dir");
       //Now we find all the files besides "."/".."
       DIR *dirp = (opendir(argv[2]));
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

   //get width  
   int width = atoi(argv[1]);
   //wrap with file name and width
   wrap(argv[2],width); 
   
   return EXIT_SUCCESS;
}
