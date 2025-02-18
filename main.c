#include <curses.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

#define MAX_CMD_CHARS 50

struct instruction {
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
 * Decodifica la instrucción leída.
 * Esta instrucción es una de las soportadas por la "arquitectura" que estamos
 * construyendo.
 */
struct instruction *
instruction_decode(const char *buf)
{
    struct instruction *inst = malloc(sizeof(*inst));
    if (inst) {
        sscanf(buf, "%s %s %s", inst->name, inst->arg1, inst->arg2);
        for(size_t i = 0; inst->name[i] != '\0'; i += 1) {
            inst->name[i] = toupper(inst->name[i]);
        }
    }
    return inst;
}

struct prompt {
    char buf[120];
    size_t buflen;
    struct instruction *decoded_inst;
    WINDOW *win;
};

enum prompt_status {
    PROMPT_STATUS_OK,
    PROMPT_STATUS_INSTRUCTION_DECODED
};

/*
 * Actualiza la información del prompt del simulador, regresa una instrucción
 * en caso de habérsele dictado una.
 */
enum prompt_status
prompt_update(struct prompt *prompt)
{
    static char buf[120];
    static int32_t buflen = 0;
    char c;
    enum prompt_status status = PROMPT_STATUS_OK;
    if ((c = wgetch(prompt->win)) != -1) {
        if (c == '\n') { 
            prompt->decoded_inst = instruction_decode(buf);
            buf[0] = buflen = 0;
            status = PROMPT_STATUS_INSTRUCTION_DECODED;
        } 
        else if (c == 127 || c == 8) {
            if (buflen > 0) {
                buf[buflen - 1] = 0;
                buflen -= 1;
            }
        }
        else if (buflen < 120) {
            buf[buflen] = c;
            buf[buflen + 1] = 0;
            buflen += 1;
        }
        wclear(prompt->win);
        mvwprintw(prompt->win, 6, 0, "$ %s", buf);
        wrefresh(prompt->win);
    }
    return status;
}

/*
 * Evalua el comando pasado como argumento.
 * Todas las acciones que se podrán realizar desde el prompt serán
 * implementadas en esta función.
 */
int32_t
eval(struct instruction *cmd) {
    if (!cmd) { return -1; }
    if (strncmp(cmd->name, "EXIT", 4) == 0) {
        endwin();
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
    initscr();
    noecho();
    cbreak(); 

    WINDOW *messages = newwin(10, 80, 0, 0);
    WINDOW *registers = newwin(7, 80, 10, 0);
    struct prompt prompt;
    prompt.win = newwin(7, 80, 17, 0);
    nodelay(prompt.win, TRUE); 

    for (int i = 0; i < 1000; i += 1) {
        wprintw(registers, "*");
    }
    wrefresh(registers);

    char buf[80] = { 0 };
    while (true) {
        if (prompt_update(&prompt) == PROMPT_STATUS_INSTRUCTION_DECODED) {
            eval(prompt.decoded_inst);
            free(prompt.decoded_inst);
        }
        usleep(16E3);
    }
    return 0;
}
