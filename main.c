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

typedef struct {
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    char IR[100];
} PCB;

void mostrarPCB(PCB *pcb) {
    printf("AX: %d\n", pcb->AX);
    printf("BX: %d\n", pcb->BX);
    printf("CX: %d\n", pcb->CX);
    printf("DX: %d\n", pcb->DX);
    printf("PC: %d\n", pcb->PC);
    printf("IR: %s\n", pcb->IR);
}

void inicializarPCB(PCB *pcb) {
    pcb->AX = 0;
    pcb->BX = 0;
    pcb->CX = 0;
    pcb->DX = 0;
    pcb->PC = 0;
    memset(pcb->IR, 0, sizeof(pcb->IR));
}

// Función para procesar una instrucción
void procesar_instruccion(PCB *pcb, const char *instr, WINDOW *messages) {
    char op[4], p1[10], p2[10];
    int val1, val2;

    usleep(16E3);

    // Actualizar el campo IR del PCB con la instrucción actual
    strncpy(pcb->IR, instr, sizeof(pcb->IR) - 1);
    pcb->IR[sizeof(pcb->IR) - 1] = '\0';

    sscanf(instr, "%s %s %s", op, p1, p2);
    // Pasar a mayúsculas los strings para evitar errores
    for(size_t i = 0; op[i] != '\0'; i += 1) {
        op[i] = toupper(op[i]);
    }
    for(size_t i = 0; p1[i] != '\0'; i += 1) {
        p1[i] = toupper(p1[i]);
    }
    for(size_t i = 0; p2[i] != '\0'; i += 1) {
        p2[i] = toupper(p2[i]);
    }

    if (strcmp(op, "MOV") == 0) {
        val2 = isdigit(p2[0]) ? atoi(p2) : (strcmp(p2, "AX") == 0 ? pcb->AX : strcmp(p2, "BX") == 0 ? pcb->BX : strcmp(p2, "CX") == 0 ? pcb->CX : strcmp(p2, "DX") == 0 ? pcb->DX : 0);
        if (strcmp(p1, "AX") == 0) pcb->AX = val2;
        else if (strcmp(p1, "BX") == 0) pcb->BX = val2;
        else if (strcmp(p1, "CX") == 0) pcb->CX = val2;
        else if (strcmp(p1, "DX") == 0) pcb->DX = val2;
    } else if (strcmp(op, "ADD") == 0) {
        val2 = isdigit(p2[0]) ? atoi(p2) : (strcmp(p2, "AX") == 0 ? pcb->AX : strcmp(p2, "BX") == 0 ? pcb->BX : strcmp(p2, "CX") == 0 ? pcb->CX : strcmp(p2, "DX") == 0 ? pcb->DX : 0);
        if (strcmp(p1, "AX") == 0) pcb->AX += val2;
        else if (strcmp(p1, "BX") == 0) pcb->BX += val2;
        else if (strcmp(p1, "CX") == 0) pcb->CX += val2;
        else if (strcmp(p1, "DX") == 0) pcb->DX += val2;
    } else if (strcmp(op, "SUB") == 0) {
        val2 = isdigit(p2[0]) ? atoi(p2) : (strcmp(p2, "AX") == 0 ? pcb->AX : strcmp(p2, "BX") == 0 ? pcb->BX : strcmp(p2, "CX") == 0 ? pcb->CX : strcmp(p2, "DX") == 0 ? pcb->DX : 0);
        if (strcmp(p1, "AX") == 0) pcb->AX -= val2;
        else if (strcmp(p1, "BX") == 0) pcb->BX -= val2;
        else if (strcmp(p1, "CX") == 0) pcb->CX -= val2;
        else if (strcmp(p1, "DX") == 0) pcb->DX -= val2;
    } else if (strcmp(op, "MUL") == 0) {
        val2 = isdigit(p2[0]) ? atoi(p2) : (strcmp(p2, "AX") == 0 ? pcb->AX : strcmp(p2, "BX") == 0 ? pcb->BX : strcmp(p2, "CX") == 0 ? pcb->CX : strcmp(p2, "DX") == 0 ? pcb->DX : 0);
        if (strcmp(p1, "AX") == 0) pcb->AX *= val2;
        else if (strcmp(p1, "BX") == 0) pcb->BX *= val2;
        else if (strcmp(p1, "CX") == 0) pcb->CX *= val2;
        else if (strcmp(p1, "DX") == 0) pcb->DX *= val2;
    } else if (strcmp(op, "DIV") == 0) {
        val2 = isdigit(p2[0]) ? atoi(p2) : (strcmp(p2, "AX") == 0 ? pcb->AX : strcmp(p2, "BX") == 0 ? pcb->BX : strcmp(p2, "CX") == 0 ? pcb->CX : strcmp(p2, "DX") == 0 ? pcb->DX : 0);
        if (val2 != 0) {
            if (strcmp(p1, "AX") == 0) pcb->AX /= val2;
            else if (strcmp(p1, "BX") == 0) pcb->BX /= val2;
            else if (strcmp(p1, "CX") == 0) pcb->CX /= val2;
            else if (strcmp(p1, "DX") == 0) pcb->DX /= val2;
        } else {
            mvwprintw(messages, 1, 1, "Error: Division por cero");
            wrefresh(messages);
        }
    } else if (strcmp(op, "INC") == 0) {
        if (strcmp(p1, "AX") == 0) pcb->AX++;
        else if (strcmp(p1, "BX") == 0) pcb->BX++;
        else if (strcmp(p1, "CX") == 0) pcb->CX++;
        else if (strcmp(p1, "DX") == 0) pcb->DX++;
    } else if (strcmp(op, "DEC") == 0) {
        if (strcmp(p1, "AX") == 0) pcb->AX--;
        else if (strcmp(p1, "BX") == 0) pcb->BX--;
        else if (strcmp(p1, "CX") == 0) pcb->CX--;
        else if (strcmp(p1, "DX") == 0) pcb->DX--;
    } else if (strcmp(op, "END") == 0) {
        endwin();
        //exit(0);
    } else {
        mvwprintw(messages, 1, 1, "Error: Instruccion %s no reconocida\n", instr);
        wrefresh(messages);
    }

    pcb->PC++;
}

/* Prototipos de cosas a definir */

/* Estructura utilizada para guardar el historial de los textos a mostrar en
 * mensajes y prompt.
 * Esta debería ser una cola circular.
 */

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
        inst->name[0] = '\0';
        inst->arg1[0] = '\0';
        inst->arg2[0] = '\0';
        sscanf(buf, "%s %s %s", inst->name, inst->arg1, inst->arg2);
        for(size_t i = 0; inst->name[i] != '\0'; i += 1) {
            inst->name[i] = toupper(inst->name[i]);
        }
        for(size_t i = 0; inst->arg1[i] != '\0'; i += 1) {
            inst->arg1[i] = toupper(inst->arg1[i]);
        }
        for(size_t i = 0; inst->arg2[i] != '\0'; i += 1) {
            inst->arg2[i] = toupper(inst->arg2[i]);
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
                snprintf(buf, sizeof(buf), "%s %s %s", prompt->hist.history[prompt->hist_index].name, prompt->hist.history[prompt->hist_index].arg1, prompt->hist.history[prompt->hist_index].arg2);
                buflen = strlen(buf);
            }
        } else if (c == KEY_DOWN) {
            if (prompt->hist.size > 0) {
                prompt->hist_index = (prompt->hist_index + 1) % HISTORY_SIZE;
                snprintf(buf, sizeof(buf), "%s %s %s", prompt->hist.history[prompt->hist_index].name, prompt->hist.history[prompt->hist_index].arg1, prompt->hist.history[prompt->hist_index].arg2);
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

void instruccions_win(WINDOW *win, PCB *pcb) {
    box(win, 0, 0); 

    mvwprintw(win, 0, 35, "|Registers|");

    mvwprintw(win, 2, 2, "AX: %d", pcb->AX);
    mvwprintw(win, 3, 2, "BX: %d", pcb->BX);
    mvwprintw(win, 4, 2, "CX: %d", pcb->CX);
    mvwprintw(win, 2, 35, "DX: %d", pcb->DX);
    mvwprintw(win, 3, 35, "PC: %d", pcb->PC);
    mvwprintw(win, 4, 35, "IR: %s", pcb->IR);

    wrefresh(win);
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
eval(struct instruction *cmd, WINDOW *messages, PCB *pcb) {
    if (!cmd) { 
        clear_window_part(messages, 1,1,8,78);
        mvwprintw(messages, 1, 1, "Error: Comando no reconocido");
        wrefresh(messages);
        return -1;
    }
    if (strncmp(cmd->name, "EXIT", 4) == 0) {
        clear_window_part(messages, 1,1,8,78);
        wrefresh(messages);
        mvwprintw(messages, 1, 1, "Saliendo...");
        wrefresh(messages);
        usleep(90E4);
        endwin();
        exit(0);
    }   
    else if (strncmp(cmd->name, "LOAD", 4) == 0) {
        if (cmd->arg1[0] == '\0') {
            clear_window_part(messages, 1,1,8,78);
            mvwprintw(messages, 1, 1, "Error: Falta nombre de archivo para LOAD");
            wrefresh(messages);
            return 1;
        }
        else {
            inicializarPCB(pcb);
            clear_window_part(messages, 1,1,8,78);
            mvwprintw(messages, 1, 1, "Cargando archivo %s\n", cmd->arg1);
            wrefresh(messages);
            usleep(90E4);
            FILE *file = fopen(cmd->arg1, "r");
            if (file) {
                int line_num = 1;
                char line[256];
                while (fgets(line, sizeof(line), file)) {
                    line[strcspn(line, "\n")] = '\0'; // Eliminar el salto de línea
                    procesar_instruccion(pcb, line, messages);
                    mvwprintw(messages, line_num, 1, "%s", line);
                    box(messages, 0, 0);
                    mvwprintw(messages, 0, 35, "|Messages|");
                    wrefresh(messages);
                    line_num++;
                    if (line_num > 8) {
                        line_num = 1;
                        usleep(20E4);
                        clear_window_part(messages, 1,1,8,78);
                    }
                }
                fclose(file);
            } 
            else {
                clear_window_part(messages, 1,1,1,78);
                mvwprintw(messages, 1, 1, "Error: No se pudo abrir el archivo %s\n", cmd->arg1);
                wrefresh(messages);
            }
        }
    }
    else {
        clear_window_part(messages, 1,1,1,78);
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

    PCB pcb;
    inicializarPCB(&pcb);

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
        instruccions_win(registers, &pcb);
        messages_win(messages);
        if (prompt_update(&prompt) == PROMPT_STATUS_INSTRUCTION_DECODED) {
            eval(prompt.decoded_inst, messages, &pcb);
            free(prompt.decoded_inst);
        }
        usleep(16E3);
    }
    endwin();
    return 0;
}
