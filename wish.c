#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define LEN 5
#define LEN1 100

int main(){
    char *command = NULL, cmd[LEN1], pathvar[LEN1];
    char *args[LEN] = {NULL}, *path[LEN] = {NULL};
    size_t size = 0;
    int accessed = 0;

    while (1){
        printf("wish> ");
        getline(&command, &size, stdin);
        /*
        ylimääräisen rivinvaihdon poisto
        https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        */
        command[strcspn(command, "\n")] = 0;
        strcpy(cmd, strtok(command, " "));

        args[0] = cmd;

        for (int i = 1; i < LEN; i++){
            args[i] = strtok(NULL, " ");
        }

        if (strcmp(cmd, "path") != 0){
            printf("\n\nKaikki muuttujat:\ncommand: %s\t\tcmd: %s\t\tpathvar: %s\n", command, cmd, pathvar);
            for (int i = 0; i < LEN; i++){
                printf("args[%d]: %s\t\tpath[%d]: %s\n", i, args[i], i, path[i]);
            }
            printf("size: %ld\t\taccessed: %d", size, accessed);
            printf("\n\n");
        }

        if (strcmp(cmd, "exit") == 0){
            exit(0);

        } else if (strcmp(cmd, "cd") == 0){
            if ((args[1] == NULL) || (args[2] != NULL)){
                fprintf(stderr, "Usage: cd <argument>\n");
            } else {
                chdir(args[1]);
            }

        } else if (strcmp(cmd, "path") == 0){
            memcpy(path, args, sizeof(args));

            printf("\n\nKaikki muuttujat:\ncommand: %s\t\tcmd: %s\t\tpathvar: %s\n", command, cmd, pathvar);
            for (int i = 0; i < LEN; i++){
                printf("args[%d]: %s\t\tpath[%d]: %s\n", i, args[i], i, path[i]);
            }
            printf("size: %ld\t\taccessed: %d", size, accessed);
            printf("\n\n");

        } else {
            if (path[1] == NULL){
                fprintf(stderr, "Error: could not find path\n");
                continue;
            }
            for (int i = 1; i < LEN-1; i++) {    // path[0] on aina null
                if (path[i] == NULL){
                    break;
                }
                strcpy(pathvar, path[i]);
                strcat(pathvar, command);      
                if (access(pathvar, X_OK) == 0){
                    accessed = 1;
                    int rc = fork();
                    if (rc < 0){
                        fprintf(stderr, "fork failed\n");
                        exit(1);
                    } else if (rc == 0){    //lapsi (uusi prosessi)
                        execv(pathvar, args);
                    } else {                // vanhempi
                        wait(NULL);
                    }
                    break;
                } else {
                    accessed = 0;
                }
            }
            if (!accessed){
                fprintf(stderr, "Error: command '%s' not found\n", cmd);
            }
        }   
    }
}