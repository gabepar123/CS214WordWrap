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

char* createFileName(char fileName[]){
    //create the new file name with the "wrap." prefix
    char* prefix = "wrap.";
    
    int prefixLength = strlen(prefix);
    int fileNameLength = strlen(fileName);
  
    int newFileLength = prefixLength + fileNameLength + 1;
    char* newFileName = malloc(newFileLength * sizeof(char));

    strcpy(newFileName, prefix);
    strcat(newFileName, fileName);

    return newFileName;
}

int isDirectory(char *name){
    struct stat data;

    stat(name, &data);
    /*
    //check for errors
    int error = stat(name, &data);
    if (error){
        perror(name);
        return 0;
    }
    */

    //check if argv is a directory
    if (S_ISDIR(data.st_mode)){
        return 1;
    }
    return 0;

}

int wrap(int fin, int fout, unsigned width){

    int bytes_read;
    strbuf_t buf;
    strbuf_t temp;
    sb_init(&buf,10);
    sb_init(&temp,10);
    bytes_read = read(fin,buf.data,10);

    if(bytes_read==0)
        return 0;

    int i;
    int totalBytes=0;
    int tooBig = 0;
    int correctBytes=0;
    while(bytes_read!=0){
        int startIndex=0;
        int started=0;
        int startedSpace=0;
        int wordBytes=0;
        for(i=0;i<10;++i){
            
            //if it is a space, then we know the word has ended
            if(isspace(buf.data[i])){
                if(wordBytes>width){//the word that we are writing is longer than the width allowed
                    tooBig = 1;
                    write(fout, buf.data+startIndex, wordBytes);
                    write(fout,"\n",1);
                }
                else{
                    //write previous content to the output file
                    write(fout, buf.data+startIndex, wordBytes);
                }
                //say that we did not start at a word
                started=0;
                //clear about of wordBytes
                wordBytes=0;
                //check if its a new line
                if(buf.data[i]=='\n'||buf.data[i]=='\t'){
                    buf.data[i]=' ';
                }
                //check if we in a space chunk
                if(startedSpace==0){
                    startIndex=i;
                    wordBytes=1;
                    startedSpace=1;
                }
                else{//in middle of space chunk
                    wordBytes++;
                }
                //totalBytes++;
            }
            else{//if not a space, check if we are in the middle of a word
                
                if(started==0){//not in the middle of a word
                    if(totalBytes!=wordBytes)
                        write(fout, buf.data+startIndex,wordBytes);
                    startedSpace=0;
                    startIndex=i;
                    wordBytes=1;
                    started=1;
                }
                else{//in middle of word
                    wordBytes++;
                }
            }
            totalBytes++;
            if(totalBytes>=width){
                write(fout,"\n",1);
                //if the amount we read is equal/greater than the amount we want to wrap, then break and change totalBytes
                totalBytes=wordBytes;
                //break;
            }
        }//end of buffer reading
        //if we are in a word right now but the buffer has ended, stash it
        correctBytes=buf.used;
        if(started==1||startedSpace==1){
            int j;
            int x = startIndex;
            //temp is the rest of the word in the buffer
            for(j=0;j<wordBytes;++j){
                sb_append(&temp,buf.data[x]);
                ++x;
            }
        }

        sb_destroy(&buf);
        sb_init(&buf,10);
        int k;
        for(k=0;k<wordBytes;++k){
                sb_append(&buf,temp.data[k]);
        }
        bytes_read = read(fin,buf.data+temp.used,10-(temp.used));
        if(started==1||startedSpace==1)
            totalBytes-=temp.used;

        buf.used+=bytes_read;
        
        if(bytes_read==0){
            write(fout,buf.data,correctBytes);
        }

        sb_destroy(&temp);
        sb_init(&temp,10);

    }//end of file reading

    sb_destroy(&buf);
    sb_destroy(&temp);
    write(fout,"\n",1);
    if(tooBig==1)
        return 1;

    return 0;
}

int main(int argc, char* argv[]){

    //Check if proper arguments are given. If it is > 3 , then return with EXIT_FAILURE.
    //TODO: argc can be 2 since we can take input from stdio
    if(argc > 3)
        return EXIT_FAILURE;
        
    //get width  
    unsigned width = atoi(argv[1]);
    
    //If there is only 2 arguments, we read from stdin
    if (argc == 2){
        int fin = 0;
        int fout = 1;
        int w3=wrap(fin,fout,width);
        if(w3==1)
            return EXIT_FAILURE;
 
        return EXIT_SUCCESS;
    }

    //if argv is a directory, word wrap all the files in it
   if (isDirectory(argv[2])) {
        DIR *dirp = (opendir(argv[2]));
        struct dirent *de;
        chdir(argv[2]);
        while ((de = readdir(dirp)) != NULL) {
            //We check if the file is a regular file, and does not start with "." or "wrap."
            if (de->d_type == DT_REG && ignoreFileName(de->d_name)) {
                //Open the input file
                int fin = open(de->d_name, O_RDONLY);
                if (fin == -1){ perror(de->d_name); return EXIT_FAILURE;}
                //create wrap. file name
                char* newFileName = createFileName(de->d_name);
                //0600 gives rw------
                //Output file (wrap.foo)
                int fout = open(newFileName, O_RDWR|O_TRUNC|O_CREAT, 0600);
                if (fout == -1){ perror(newFileName); return EXIT_FAILURE;}
                free(newFileName);
                int w2 = wrap(fin,fout,width);
                if(w2==1)
                    return EXIT_FAILURE;
           }
       }
       closedir(dirp);
   }
   else {
      int fin = open(argv[2], O_RDONLY); 
      if (fin == -1){ perror(argv[2]); return EXIT_FAILURE;}
      int fout = 1;
      int w = wrap(fin,fout,width);
      if(w==1)
        return EXIT_FAILURE;
   }

   return EXIT_SUCCESS;
}
