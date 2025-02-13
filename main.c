#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "inmanip.h"

struct cmd {
    char name[50];
    char arg1[50];
    char arg2[50];
};

/*
 * Imprime un prompt y obtiene un comando ingresado por el usuario.
 */
struct cmd
prompt(void)
{
    struct cmd cmd;
    char buf[80] = { 0 };
    int32_t buflen = 0;
    char c;
    while (true) {
        printf("$ %.*s\n", buflen, buf);
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
eval(struct cmd *cmd) {
    if (!cmd) { return -1; }
    if (strncmp(cmd->name, "EXIT", 4) == 0 ||
        strncmp(cmd->name, "SALIR", 5) == 0) {
        return 0;
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
                while (fgets(line, sizeof(line), file)) {
                    printf("%s", line);
                }
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
