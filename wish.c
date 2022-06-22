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
    char *command = NULL, cmd[CMDLEN], pathvar[CMDLEN], *token;
    char *args[ARGLEN] = {NULL}, *path[PATHLEN] = {NULL};
    size_t size = 0;
    int accessed = 0;

    while (1)
    {
        for (int i = 0; i < ARGLEN; i++) // args-listan tyhjennys ja muistin vapautus
        {
            if (args[i] != NULL)
            {
                free(args[i]);
                args[i] = NULL;
            }
        }

        printf("wish> ");
        getline(&command, &size, stdin);
        /*
        ylimääräisen rivinvaihdon poisto
        https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input
        */
        command[strcspn(command, "\n")] = 0;
        strcpy(cmd, strtok(command, " "));

        // Muistin varaus args-listalle riviin 59 asti
        if (args[0] == NULL)
        {
            args[0] = malloc(sizeof(char) * 50);
        }
        else if (args[1] != NULL)
        {
            args[0] = realloc(args[0], sizeof(char) * 50);
        }

        strcpy(args[0], cmd);
        token = strtok(NULL, " ");

        int counter = 1;
        while (token != NULL)
        {
            args[counter] = malloc(sizeof(char) * 50);
            strcpy(args[counter], token);

            token = strtok(NULL, " ");
            counter++;
        }

        if (strcmp(cmd, "exit") == 0)
        {
            for (int i = 0; i < ARGLEN; i++) // args-listan tyhjennys ja muistin vapautus
            {
                if (args[i] != NULL)
                {
                    free(args[i]);
                    args[i] = NULL;
                }
            }

            for (int i = 0; i < PATHLEN; i++) // path-listan tyhjennys ja muistin vapautus
            {
                if (path[i] != NULL)
                {
                    free(path[i]);
                    path[i] = NULL;
                }
            }
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
            for (int i = 0; i < PATHLEN; i++) // Muistin varaus/vapautus path-listalle
            {
                if ((path[i] == NULL) && (args[i + 1] != NULL))
                {
                    path[i] = malloc(sizeof(char) * 50);
                    strcpy(path[i], args[i + 1]);
                }
                else if ((path[i] != NULL) && (args[i + 1] == NULL))
                {
                    free(path[i]);
                    path[i] = NULL;
                }
                else if ((path[i] != NULL) && (args[i + 1] != NULL))
                {
                    path[i] = realloc(path[i], sizeof(char) * 50);
                    strcpy(path[i], args[i + 1]);
                }
            }
        }
        else
        {
            if (path[0] == NULL)
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