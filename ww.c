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
    strbuf_t words;
    sb_init(&words,width);
    sb_init(&buf,15);
    sb_init(&temp,15);
    bytes_read = read(fin,buf.data,15);

    if(bytes_read==0){
        sb_destroy(&buf);
        sb_destroy(&temp);
        return 0;
    }

    int i;
    int totalBytes=0;
    int tooBig = 0;
    int correctBytes=0;
    while(bytes_read!=0){
        int startIndex=0;
        int startedWord=0;
        int startedSpace=0;
        int wordBytes=0;
        for(i=0;i<15;++i){
            //if it is a space, then we know the word has ended
            if(isspace(buf.data[i])){
                if(wordBytes>width){//the word that we are writing is longer than the width allowed
                    tooBig = 1;
                    write(fout, buf.data+startIndex, wordBytes);
                    write(fout,"\n",1);
                }
                else{
                    //write previous content to the output file
                    //write(fout, buf.data+startIndex, wordBytes);
                    int x;
                    for(x=0;x<wordBytes;++x){
                        sb_append(&words,buf.data[startIndex+x]);
                    }
                    wordBytes=0;
                    startedWord=0;
                }

                
                
                //say that we did not start at a word
                //clear about of wordBytes
                
                //check if its a new line
                //if((buf.data[i]=='\t'||buf.data[i]=='\n')&&totalBytes!=0){//if the char is a tab and it isnt the first char in the line, make it a space
                //    buf.data[i]=' ';
                //}
                
                //check if we in a space chunk
                if(startedSpace==0){//not in space chunk
                    startIndex=i;
                    wordBytes=1;
                    startedSpace=1;
                }
                else{//in middle of space chunk
                    wordBytes++;
                }
            }
            else{//if not a space, check if we are in the middle of a word
                
                if(startedWord==0){//not in the middle of a word, starting a new word
                    if(totalBytes!=wordBytes){
                        //write(fout, buf.data+startIndex,wordBytes);

                        //check if its just one space in our space chunk
                        if(wordBytes==1){
                            if(buf.data[i-1]=='\n'||buf.data[i-1]=='\t')
                                buf.data[i-1]=' ';
                        }
                        int x;
                        for(x=0;x<wordBytes;++x){
                            sb_append(&words,buf.data[startIndex+x]);
                        }
                    }
                    startedSpace=0;
                    startIndex=i;
                    wordBytes=1;
                    startedWord=1;
                }
                else{//in middle of word
                    wordBytes++;
                }
            }
            totalBytes++;
            if(totalBytes>=width){
                if((i<9&&buf.data[1+i]==' ')){
                    int x;
                    for(x=0;x<wordBytes;++x){
                        sb_append(&words,buf.data[startIndex+x]);
                    }
                    startedWord=0;
                    wordBytes=0;
                    startIndex=i+1;
                }
                while(words.used>0&&words.data[words.used-1]==' '){
                    --words.used;
                }
                if(words.used!=0){
                    
                    write(fout,words.data,words.used);
                    write(fout,"\n",1);
                }
                sb_destroy(&words);
                sb_init(&words,width);
                //if the amount we read is equal/greater than the amount we want to wrap, then break and change totalBytes
                totalBytes=wordBytes;
            }
        }//end of buffer reading
        //if we are in a word right now but the buffer has ended, stash it
        correctBytes=buf.used;
        if(startedWord==1||startedSpace==1){
            int j;
            int x = startIndex;
            //temp is the rest of the word in the buffer
            for(j=0;j<wordBytes;++j){
                sb_append(&temp,buf.data[x]);
                ++x;
            }
        }

        sb_destroy(&buf);
        sb_init(&buf,15);
        int k;
        for(k=0;k<wordBytes;++k){
                sb_append(&buf,temp.data[k]);
        }
        bytes_read = read(fin,buf.data+temp.used,15-(temp.used));
        if(startedWord==1||startedSpace==1)
            totalBytes-=temp.used;

        buf.used+=bytes_read;
        
        if(bytes_read==0){
            write(fout,words.data,words.used);
            write(fout,buf.data,correctBytes);
        }

        sb_destroy(&temp);
        sb_init(&temp,15);

    }//end of file reading

    sb_destroy(&buf);
    sb_destroy(&temp);
    sb_destroy(&words);
    write(fout,"\n",1);
    if(tooBig==1)
        return 1;

    return 0;
}

int main(int argc, char* argv[]){

    //Check if proper arguments are given. If it is > 3 , then return with EXIT_FAILURE.
    if(argc > 3)
        return EXIT_FAILURE;
        
    //get width  
    unsigned width = atoi(argv[1]);

    if(width==0)
        return EXIT_FAILURE;
    
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
