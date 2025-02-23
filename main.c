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
#define HISTORY_SIZE 3

/* Prototipos de cosas a definir */

/* Estructura utilizada para guardar el historial de los textos a mostrar en
 * mensajes y prompt.
 * Esta debería ser una cola circular.
 */
struct log {
    char strings[10][50];
    size_t capacity;
    size_t last;
};

/* Crea una nueva instancia de registro, utilizado para prompt y mensajes
 */
struct log *log_new();

/* Registra una nueva cadena de texto en el registro, elimina la cadena más
 * vieja en caso de estar ya lleno.
 */
int32_t log_log(char str[50]) { }

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

// Función para limpiar una parte específica de una ventana sin borrar el marco
void clear_window_part(WINDOW *win, int start_y, int start_x, int height, int width) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            mvwaddch(win, y, x, ' ');
        }
    }
    wrefresh(win);
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

struct instruction_history {
    struct instruction history[HISTORY_SIZE];
    int current;
    int size;
};

struct prompt {
    char buf[120];
    size_t buflen;
    struct instruction *decoded_inst;
    WINDOW *win;
    struct instruction_history hist;
    int hist_index;
};

enum prompt_status {
    PROMPT_STATUS_OK,
    PROMPT_STATUS_INVALID,
    PROMPT_STATUS_INSTRUCTION_DECODED
};

/*
 * Actualiza la información del prompt del simulador, regresa una instrucción
 * en caso de habérsele dictado una.
 */
enum prompt_status
prompt_update(struct prompt *prompt)
{
    static char buf[80];
    static int32_t buflen = 0;
    int c;
    enum prompt_status status = PROMPT_STATUS_OK;
    if ((c = wgetch(prompt->win)) != -1) {
        if (c == '\n') { 
            clear_window_part(prompt->win, 1, 1, 5, 78);
            prompt->decoded_inst = instruction_decode(buf);
            buf[0] = buflen = 0;
            if (prompt->decoded_inst) {
                // Add instruction to history
                prompt->hist.history[prompt->hist.current] = *prompt->decoded_inst;
                prompt->hist.current = (prompt->hist.current + 1) % HISTORY_SIZE;
                if (prompt->hist.size < HISTORY_SIZE) {
                    prompt->hist.size++;
                }
                prompt->hist_index = prompt->hist.current;
                status = PROMPT_STATUS_INSTRUCTION_DECODED;
            } else {
                status = PROMPT_STATUS_INVALID;
            }
        } else if (c == KEY_BACKSPACE || c == 127 || c == 8) {
            if (buflen > 0) {
                buf[buflen - 1] = 0;
                buflen -= 1;
            }
        } else if (c == KEY_UP) {
            if (prompt->hist.size > 0) {
                prompt->hist_index = (prompt->hist_index - 1 + HISTORY_SIZE) % HISTORY_SIZE;
                strncpy(buf, prompt->hist.history[prompt->hist_index].name, sizeof(buf));
                buflen = strlen(buf);
            }
        } else if (c == KEY_DOWN) {
            if (prompt->hist.size > 0) {
                prompt->hist_index = (prompt->hist_index + 1) % HISTORY_SIZE;
                strncpy(buf, prompt->hist.history[prompt->hist_index].name, sizeof(buf));
                buflen = strlen(buf);
            }
        } else if (buflen < 79) {
            buf[buflen] = c;
            buf[buflen + 1] = 0;
            buflen += 1;
        }
        clear_window_part(prompt->win, 5, 1, 1, 78);
        wrefresh(prompt->win);
        mvwprintw(prompt->win, 5, 1, "$ %s", buf);
    }
    return status;
}

void instruccions_win(WINDOW *registers) {
    box(registers, 0, 0); 

    mvwprintw(registers, 0, 35, "|Registers|");

    mvwprintw(registers, 2, 2, "AX: ");
    mvwprintw(registers, 3, 2, "BX: ");
    mvwprintw(registers, 4, 2, "CX: ");
    mvwprintw(registers, 2, 38, "DX: ");
    mvwprintw(registers, 3, 38, "PC: ");
    mvwprintw(registers, 4, 38, "IR: ");

    wrefresh(registers);
}

void messages_win(WINDOW *messages) {
    box(messages, 0, 0); 

    mvwprintw(messages, 0, 35, "|Messages|");

    wrefresh(messages);
}

/*
 * Evalua el comando pasado como argumento.
 * Todas las acciones que se podrán realizar desde el prompt serán
 * implementadas en esta función.
 */
int32_t 
eval(struct instruction *cmd, WINDOW *messages) {
    if (!cmd) { 
        mvwprintw(messages, 1, 1, "Error: Comando no reconocido");
        wrefresh(messages);
        return -1;
    }
    if (strncmp(cmd->name, "EXIT", 4) == 0) {
        mvwprintw(messages, 1, 1, "Saliendo...");
        wrefresh(messages);
        usleep(90E4);
        endwin();
        exit(0);
    }
    else if (strncmp(cmd->name, "LOAD", 4) == 0) {
        if (cmd->arg1[0] == '\0') {
            mvwprintw(messages, 1, 1, "Error: Falta nombre de archivo para LOAD");
            wrefresh(messages);
            return 1;
        }
        else {
            mvwprintw(messages, 1, 1, "Cargando archivo %s\n", cmd->arg1);
            wrefresh(messages);
            usleep(90E4);
            FILE *file = fopen(cmd->arg1, "r");
            if (file) {
                char line[256];
                while (!fprintline(file));
                fclose(file);
            }
            else {
                mvwprintw(messages, 1, 1, "Error: No se pudo abrir el archivo %s\n", cmd->arg1);
                wrefresh(messages);
            }
        }
    }
    else {
        mvwprintw(messages, 1, 1, "Error: Comando %s no reconocido",cmd->name);
        wrefresh(messages);
        return 1;
    }
}

int
main(void)
{
    initscr();
    noecho();
    cbreak(); 
    curs_set(0);

    WINDOW *messages = newwin(10, 80, 0, 0);
    WINDOW *registers = newwin(7, 80, 10, 0);
    struct prompt prompt;
    prompt.win = newwin(7, 80, 17, 0);
    nodelay(prompt.win, TRUE); 
    keypad(prompt.win, TRUE);
    prompt.hist.current = 0;
    prompt.hist.size = 0;
    prompt.hist_index = 0;

    box(prompt.win, 0, 0);
    mvwprintw(prompt.win, 0, 35, "|Prompt|");

    char buf[80] = { 0 };
    while (true) {
        instruccions_win(registers);
        messages_win(messages);
        if (prompt_update(&prompt) == PROMPT_STATUS_INSTRUCTION_DECODED) {
            eval(prompt.decoded_inst, messages);
            free(prompt.decoded_inst);
        }
        usleep(16E3);
    }
    return 0;
}