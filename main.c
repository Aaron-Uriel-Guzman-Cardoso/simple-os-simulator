#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "inmanip.h"

struct cmd {
    char name[50];
    char arg1[50];
    char arg2[50];
} typedef CMD;

/*
 * Lee la siguiente línea del archivo y la evalua (imprime)
 */
int32_t
fprintline(FILE *file) {
    if (!file) { return -1; }
    char line[256];
    if (fgets(line, sizeof(line), file)) {
        printf("%s", line);
        return 0;
    }
    return 1;
}

/*
 * Imprime un prompt y obtiene un comando ingresado por el usuario.
 */
CMD prompt(void)
{
    CMD cmd;
    char buf[80] = { 0 };
    char tempBuf[80] = { 0 }; //buffer auxiliar para comparar
    tempBuf[0] = 'a';
    int32_t buflen = 0;
    char c;
    while (true) {
        
        if(strcmp(tempBuf, buf) != 0) {
            printf("\n>> ");
            printf("%.*s", buflen, buf);
            strcpy(tempBuf, buf);
        } // se imprime el prompt solo si los dos buffer son distintos (se ha escrito algo) y no están vacíos
        
        if (kbhit()) {
            do {
                c = getch();
                if (c == '\n') {
                    sscanf(buf, "%s %s %s", cmd.name, cmd.arg1, cmd.arg2);
                    for(int i = 0; cmd.name[i] != '\0'; i += 1) {
                        cmd.name[i] = toupper(cmd.name[i]);
                    }
                    return cmd;
                } 
                else if (c == 127) {
                    buf[buflen] = '\0'; // se vacía el buffer
                    if (buflen > 0) { buflen -= 1; }
                }
                else {
                    buf[buflen] = c;
                    buflen += 1;
                }
            } while(kbhit());
        }
    }
}

/*
 * Evalua el comando pasado como argumento.
 * Todas las acciones que se podrán realizar desde el prompt serán
 * implementadas en esta función.
 */
int32_t
eval(CMD *cmd) {
    if (!cmd) { return -1; }
    if (strncmp(cmd->name, "EXIT", 4) == 0 ||
        strncmp(cmd->name, "SALIR", 5) == 0) {
        printf("\n");
        exit(0);
    }
    else if (strncmp(cmd->name, "LOAD", 4) == 0) {
        if (cmd->arg1[0] == '\0') {
            printf("Error: Falta el nombre del archivo\n");
            return 1;
        }
        else {
            printf("Cargando archivo %s\n", cmd->arg1);
            FILE *file = fopen(cmd->arg1, "r");
            if (file) {
                char line[256];
                while (!fprintline(file));
                fclose(file);
            }
            else {
                printf("Error: No se pudo abrir el archivo %s\n",
                        cmd->arg1);
            }
        }
    }
}

int
main(void)
{
    char buf[80] = { 0 };
    int32_t buflen = 0, count = 0;
    char c;
    while (true) {
        struct cmd cmd = prompt();
        eval(&cmd);
        getchar();
    }
    return 0;
}
