#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 255

typedef struct line {
    char text[SIZE];
    struct line *nextLine;
    struct line *previousLine;
} LINE;

LINE *addLine(LINE *pFirst, char text[SIZE]){
    LINE *pNew, *ptr;

    if ((pNew = (LINE*)malloc(sizeof(LINE))) == NULL){
        fprintf(stderr, "error: malloc failed\n");
        exit(1);
    }
    strcpy(pNew->text, text);
    pNew->nextLine = NULL;
    pNew->previousLine = NULL;
    if (pFirst == NULL){
        pNew->previousLine = pFirst;
        pFirst = pNew;
    } else {
        ptr = pFirst;
        while (ptr->nextLine != NULL){
            ptr = ptr->nextLine;
        }
        pNew->previousLine = ptr;
        ptr->nextLine = pNew;
    }
    return pFirst;
}

LINE *getLinesUser(LINE *pFirst){
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t line_size;

    line_size = getline(&buffer, &buffer_size, stdin);
    while (buffer[0] != '\n'){
        pFirst = addLine(pFirst, buffer);
        line_size = getline(&buffer, &buffer_size, stdin);
    }
    free(buffer);
    buffer = NULL;
    return pFirst;
}

LINE *getLinesFile(LINE *pFirst, char *userInputFileName, int userInputSize){
    FILE *input;
    char *buffer = NULL;
    size_t buffer_size = 0;
    ssize_t line_size;

    if ((input = fopen(userInputFileName, "r")) == NULL){
        fprintf(stderr, "error: cannot open file '%s'\n", userInputFileName);
        exit(1);
    }

    // getlinen käyttö: https://riptutorial.com/c/example/8274/get-lines-from-a-file-using-getline--
    line_size = getline(&buffer, &buffer_size, input);
    while (line_size >= 0) {
        pFirst = addLine(pFirst, buffer);
        line_size = getline(&buffer, &buffer_size, input);
    }
    free(buffer);
    buffer = NULL;
    fclose(input);
    return pFirst;
}

// Vain testaamiseen
LINE *printLines(LINE *pFirst, char *outputFile){
    FILE *output;    
    LINE *ptr = pFirst;

    if (strlen(outputFile) != 0) {
        if ((output = fopen(outputFile, "w")) == NULL){
            fprintf(stderr, "cannot open file '%s'\n", outputFile);
            exit(1);
        }
    }
    while (ptr != NULL){
        if (strlen(outputFile) != 0){
            fprintf(output, "%s", ptr->text);
        } else {
            fprintf(stdout, "%s", ptr->text);
        }
        ptr = ptr->nextLine;
    }
    if (strlen(outputFile) != 0){
        fclose(output);
    }
    return pFirst;
}

LINE *printLinesReverse(LINE *pFirst, char *outputFile){
    LINE *ptr = pFirst;
    LINE *thisNode;
    FILE *output;
    
    while (ptr != NULL){
        thisNode = ptr;
        ptr = ptr->nextLine; 
    }
    /*
    En tiedä, miksei toimi kun output-filulle ei anna nimeä eli alla olevia if-lauseita
    ei suoriteta, mutta input-filulle annetaan nimi.
    */
    if (strlen(outputFile) != 0) {
        if ((output = fopen(outputFile, "w")) == NULL){
            fprintf(stderr, "error: cannot open file '%s'\n", outputFile);
            exit(1);
        }
    }

    ptr = thisNode;
    if (strlen(outputFile) != 0){
        fprintf(output, "%s", ptr->text);
    } else {
        fprintf(stdout, "%s", ptr->text);
    }

    ptr = ptr->previousLine;
    while (ptr!= NULL){
        if (strlen(outputFile) != 0){
            fprintf(output, "%s", ptr->text);
        } else {
            fprintf(stdout, "%s", ptr->text);
        }
        ptr = ptr->previousLine;
    }
    /*
    Konsoliin tulee ajettaessa "Segmentation fault" fclosen kohdalla. kts aliohjelman alussa oleva kommentti.
    Toimii ilman virheitä, jos fclosen kommentoi pois. Miksi ei toimi, jos ei anna output-filulle nimeä ja 
    input-filulle annetaan, tai jos kommentoi fclosen pois?
    */
    fclose(output);
    return pFirst;
}

LINE *freeMemory(LINE *pFirst){
    LINE *ptr = pFirst;
    while (ptr != NULL) {
        pFirst = ptr->nextLine;
        free(ptr);
        ptr = pFirst;
    }
    return pFirst;
}

int main(int argc, char *argv[]){
    LINE *pStart = NULL, *pEnd = NULL;
    char *output = "";


    if (argc > 3){
        fprintf(stderr, "usage: reverse <input> <output>\n");
        exit(1);
    }

    if ((argc == 3) && (strcmp(argv[1], argv[2]) == 0)){
        fprintf(stderr, "error: Input and output file must differ\n");
        exit(1);
    }

    if (argc == 3){
        printf("argc == 3\n");
        output = argv[2];
    }

    if (argc == 1){
        pStart = getLinesUser(pStart);
    } else {
        pStart = getLinesFile(pStart, argv[1], argc);
    }

    pStart = printLinesReverse(pStart, output);
    pStart = freeMemory(pStart);
    return 0;
}