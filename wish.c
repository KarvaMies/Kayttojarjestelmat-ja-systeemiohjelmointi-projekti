#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define ARGLEN 5
#define CMDLEN 100
#define PATHLEN 4

int main()
{
    char *command = NULL, cmd[CMDLEN], pathvar[CMDLEN];
    char *args[ARGLEN] = {NULL}, *path[PATHLEN] = {NULL};
    size_t size = 0;
    int accessed = 0;

    while (1)
    {
        // for (int i = 0; i < ARGLEN; i++)
        // {
        //     printf("ennen getlinea: path[%d]: %s\n", i, path[i]);
        // }
        printf("wish> ");
        getline(&command, &size, stdin);
        /*
        ylimääräisen rivinvaihdon poisto
        https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        */
        command[strcspn(command, "\n")] = 0;
        // for (int i = 0; i < ARGLEN; i++)
        // {
        //     printf("getlinen jälkeen: args[%d]: %s\n", i, args[i]);
        // }
        // printf("\n");

        strcpy(cmd, strtok(command, " "));
        args[0] = malloc(sizeof(char) * 50);
        args[0] = cmd;

        for (int i = 1; i < ARGLEN; i++)
        {
            args[i] = malloc(sizeof(char) * 50);
            args[i] = strtok(NULL, " ");
        }

        for (int i = 0; i < ARGLEN; i++)
        {
            printf("mallocin jälkeen: args[%d]: %s\n", i, args[i]);
        }
        printf("\n");

        // if (strcmp(cmd, "path") != 0)
        // {
        //     printf("\n\nKaikki muuttujat:\ncommand: %s\t\tcmd: %s\t\tpathvar: %s\n", command, cmd, pathvar);
        //     for (int i = 0; i < ARGLEN; i++)
        //     {
        //         printf("args[%d]: %s\t\tpath[%d]: %s\n", i, args[i], i, path[i]);
        //     }
        //     printf("size: %ld\t\taccessed: %d", size, accessed);
        //     printf("\n\n");
        // }

        if (strcmp(cmd, "exit") == 0)
        {
            exit(0);
        }
        else if (strcmp(cmd, "cd") == 0)
        {
            if ((args[1] == NULL) || (args[2] != NULL))
            {
                fprintf(stderr, "Usage: cd <argument>\n");
            }
            else
            {
                chdir(args[1]);
            }
        }
        else if (strcmp(cmd, "path") == 0)
        {
            for (int i = 0; i < PATHLEN; i++)
            {
                printf("ennen memcpy: path[%d]: %s\n", i, path[i]);
            }
            printf("\n");
            // memcpy(path, args, sizeof(args));

            for (int i = 0; i < PATHLEN; i++)
            {
                printf("Malloc-loopin alku %d\n", i);
                if (args[i + 1] != NULL)
                {
                    path[i] = malloc(sizeof(char) * 50);
                    strcpy(path[i], args[i + 1]);
                }
            }

            for (int i = 0; i < PATHLEN; i++)
            {
                printf("memcpyn jälkeen: path[%d]: %s\n", i, path[i]);
            }

            // printf("\n\nKaikki muuttujat:\ncommand: %s\t\tcmd: %s\t\tpathvar: %s\n", command, cmd, pathvar);
            // for (int i = 0; i < ARGLEN; i++)
            // {
            //     printf("args[%d]: %s\t\tpath[%d]: %s\n", i, args[i], i, path[i]);
            // }
            // printf("size: %ld\t\taccessed: %d", size, accessed);
            // printf("\n\n");
        }
        else
        {
            if (path[1] == NULL)
            {
                fprintf(stderr, "Error: could not find path\n");
                continue;
            }
            for (int i = 0; i < PATHLEN; i++)
            {
                if (path[i] == NULL)
                {
                    break;
                }
                for (int i = 0; i < PATHLEN; i++)
                {
                    printf("ls: path[%d]: %s\t args[%d]: %s\n", i, path[i], i, args[i]);
                }
                printf("\n");
                strcpy(pathvar, path[i]);
                strcat(pathvar, command);
                if (access(pathvar, X_OK) == 0)
                {
                    accessed = 1;
                    int rc = fork();
                    if (rc < 0)
                    {
                        fprintf(stderr, "fork failed\n");
                        exit(1);
                    }
                    else if (rc == 0)
                    { // lapsi (uusi prosessi)
                        execv(pathvar, args);
                    }
                    else
                    { // vanhempi
                        wait(NULL);
                    }
                    break;
                }
                else
                {
                    accessed = 0;
                }
            }
            if (!accessed)
            {
                fprintf(stderr, "Error: command '%s' not found\n", cmd);
            }
        }
    }
}
