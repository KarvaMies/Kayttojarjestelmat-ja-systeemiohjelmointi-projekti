#include <stdio.h>
#include <stdlib.h>

void readFile(char *file){
    char *buffer;

    FILE *fp = fopen(file, "r");
    if (fp == NULL){
        fprintf(stderr, "my-cat: cannot open file '%s'\n", file);
        exit(1);
    }

    buffer = malloc(255);
    while (fgets(buffer, 255, fp) != NULL){
        printf("%s", buffer);
    }
    free(buffer); 
    fclose(fp);
}

int main(int argc, char *argv[]){
    
    if (argc < 2){
        fprintf(stderr, "usage: my-cat <file>\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++){
        readFile(argv[i]);
    }
    
    return 0;
}
