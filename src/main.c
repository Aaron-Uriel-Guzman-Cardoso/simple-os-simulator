#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "inmanip.h"

#define MAX_CMD_CHARS 50

struct cmd {
    char name[MAX_CMD_CHARS];
    char arg1[MAX_CMD_CHARS];
    char arg2[MAX_CMD_CHARS];
};

/*
 * Lee la siguiente línea del archivo y la imprime
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
struct cmd *
prompt(void)
{
    struct cmd *cmd = malloc(sizeof(*cmd));
    if (!cmd) { return NULL; }
    const size_t MAX_BUF_SIZE = 120;
    char buf[MAX_BUF_SIZE], c;
    buf[0] = 0;
    int32_t buflen = 1;
    while (true) {
        printf("$ %s\n", buf);
        if (kbhit()) {
            do {
                c = getch();
                if (c == '\n') { goto entered_cmd; } 
                else if (c == 127 || c == 8) {
                    if (buflen > 0) {
                        buf[buflen - 2] = 0;
                        buflen -= 1;
                    }
                }
                else if (buflen + 1 < MAX_BUF_SIZE) {
                    buf[buflen - 1] = c;
                    buf[buflen] = 0;
                    buflen += 1;
                }
            } while (kbhit());
        }
    }
entered_cmd:
    sscanf(buf, "%s %s %s", cmd->name, cmd->arg1, cmd->arg2);
    for(int i = 0; cmd->name[i] != '\0'; i += 1) {
        cmd->name[i] = toupper(cmd->name[i]);
    }
    return cmd;
}

/*
 * Evalua el comando pasado como argumento.
 * Todas las acciones que se podrán realizar desde el prompt serán
 * implementadas en esta función.
 */
int32_t
eval(struct cmd *cmd) {
    if (!cmd) { return -1; }
    if (strncmp(cmd->name, "EXIT", 4) == 0 ||
        strncmp(cmd->name, "SALIR", 5) == 0) {
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
        struct cmd *cmd = prompt();
        eval(cmd);
        getchar();
        free(cmd);
    }
    return 0;
}
