#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "ww.h"

//TODO: Error check all file openings, mallocs, and basically anything else that can go wrong (if needed)

//determines if the file in the directory starts with '.' or "wrap."
//if it does we DO NOT word wrap it
//TODO: Test this more incase there is bugs with varying file name sizes
int ignoreFileName(char name[]){

    if (name[0] == '.' || strncmp("wrap.", name, strlen("wrap.")) == 0){
        return 0;
    }
    return 1;
}
//TODO: Finish and clean up this function
char* createFileName(char fileName[], char* path){
    //create the new file name with the "wrap." prefix
    char* prefix = "wrap.";
    
    int prefixLength = strlen(prefix);
    int fileNameLength = strlen(fileName);
    int pathFileLength = strlen(path);
    
    int newFileLength = prefixLength + fileNameLength + pathFileLength;
    
    strbuf_t newFileName;
    sb_init(&newFileName, newFileLength);
    sb_concat(&newFileName, path);
    sb_append(&newFileName, '/');
    sb_concat(&newFileName, prefix);
    sb_concat(&newFileName, fileName);


    return newFileName.data;
}

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

int wrap(int fin, int fout, unsigned width){

    int bytes_read;
    strbuf_t* buf;
    strbuf_t* temp;
    sb_init(buf,width/2);
    sb_init(temp,width/2);
    bytes_read = read(fin,&buf->data,(width/2));

    //TODO: make this algo run throughout the whole file, and not the first (width/2) bytes.
    int i;
    int started=0;
    int startIndex=0;
    int wordBytes=0;
    int totalBytes=0;
    while(bytes_read!=0){
        for(i=0;i<width/2;++i){
            if(totalBytes>=width){
                //TODO: write in a new line after every x bytes
                write(fout,"\n",1);
                //if the amount we read is equal/greater than the amount we want to wrap, then break
                break;
            }
            //if it is a space, then we know the word has ended
            if(isspace(buf->data[i])){
                //write it to the output file
                write(fout, &buf->data[startIndex], wordBytes);
                //say that we did not start at a word
                started=0;
                //clear about of wordBytes
                wordBytes=0;
            }
            else{//if not a space, check if we are in the middle of a word
                if(started==0){//not in the middle of a word
                    startIndex=i;
                    wordBytes=1;
                    started=1;
                }
                else{//in middle of word
                    wordBytes++;
                }
            }
            totalBytes++;
        }//end of buffer reading
        //if we are in a word right now but the buffer has ended, stash it
        //TODO: stash the end of a buffer properly
        //TODO: re-read bytes (outside if statement)
        if(started==1){
            int j;
            int x = startIndex;
            //temp is the rest of the word in the buffer
            for(j=0;j<wordBytes;++j){
                sb_append(temp,buf->data[x]);
                ++x;
            }
        }
        sb_destroy(buf);
        sb_init(buf,width/2);
        bytes_read = read(fin,&buf->data,(width/2));
        int k;
        for(k=0;k<wordBytes;++k){
            sb_append(buf,temp->data[k]);
        }
        sb_destroy(temp);
        sb_init(temp,width/2);

    }//end of file reading

    sb_destroy(buf);
    sb_destroy(temp);
    return 0;
}

int main(int argc, char* argv[]){

    //Check if proper arguments are given. If it is not 3 (ex: [./ww 80]), then return with EXIT_FAILURE.
    if(argc != 3)
        return EXIT_FAILURE;

    //get width  
    unsigned width = atoi(argv[1]);

    //if argv is a directory, word wrap all the files in it
   if (isDirectory(argv[2])) {
       DIR *dirp = (opendir(argv[2]));
       struct dirent *de;
        while ((de = readdir(dirp)) != NULL) {
            //We check if the file is a regular file, and does not start with "." or "wrap."
            if (de->d_type == DT_REG && ignoreFileName(de->d_name)) {
                char* newFileName = createFileName(de->d_name, argv[2]);
                //S_IRWXU gives file owner all perms, may need to change this
                int fd = open(newFileName, O_CREAT, S_IRWXU);
                free(newFileName);
                //TODO: this is the point where we call wordwrap(fd,...,...)
                wrap(,fd,width);
           }
       }
    
       closedir(dirp);
   }

   return EXIT_SUCCESS;
}
