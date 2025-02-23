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

struct log {
    char strings[10][50];
    size_t capacity;
    size_t last;
};

struct log *log_new();

int32_t log_log(char str[50]) { }

struct instruction {
    char name[MAX_CMD_CHARS];
    char arg1[MAX_CMD_CHARS];
    char arg2[MAX_CMD_CHARS];
};

int32_t fprintline(FILE *file) {
    if (!file) { return -1; }
    char line[256];
    if (fgets(line, sizeof(line), file)) {
        printf("%s", line);
        return 0;
    }
    return 1;
}

void clear_window_part(WINDOW *win, int start_y, int start_x, int height, int width) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            mvwaddch(win, y, x, ' ');
        }
    }
    wrefresh(win);
}

struct instruction *instruction_decode(const char *buf) {
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
    PROMPT_STATUS_INVALID,
    PROMPT_STATUS_INSTRUCTION_DECODED
};

struct instruction history[HISTORY_SIZE] = { 0 };
struct instruction *history_oldest = NULL;
struct instruction *history_newest = NULL;
int32_t history_count = 0;

enum prompt_status prompt_update(struct prompt *prompt, struct instruction *history) {
    static char buf[80];
    static int32_t buflen = 0;
    int c;
    enum prompt_status status = PROMPT_STATUS_OK;
    if ((c = wgetch(prompt->win)) != -1) {
        if (c == '\n') { 
            clear_window_part(prompt->win, 1,1, 5 , 78);
            prompt->decoded_inst = instruction_decode(buf);
            buf[0] = buflen = 0;
            if(prompt){
                if(history == NULL){
                    strcpy(history[history_count].name, prompt->decoded_inst->name);
                    strcpy(history[history_count].arg1, prompt->decoded_inst->arg1);
                    strcpy(history[history_count].arg2, prompt->decoded_inst->arg2);
                    history_newest = &history[history_count];
                    history_oldest = &history[history_count];
                    history_count = (history_count + 1) % HISTORY_SIZE;
                } else if(history_count < HISTORY_SIZE && history != NULL){
                    strcpy(history[history_count].name, prompt->decoded_inst->name);
                    strcpy(history[history_count].arg1, prompt->decoded_inst->arg1);
                    strcpy(history[history_count].arg2, prompt->decoded_inst->arg2);
                    history_newest = &history[history_count];
                    history_count = (history_count + 1) % HISTORY_SIZE;
                } else {
                    strcpy(history_oldest->name, prompt->decoded_inst->name);
                    strcpy(history_oldest->arg1, prompt->decoded_inst->arg1);
                    strcpy(history_oldest->arg2, prompt->decoded_inst->arg2);
                    history_oldest = &history[(history_count + 1) % HISTORY_SIZE];
                    history_newest = &history[history_count];
                    history_count = (history_count + 1) % HISTORY_SIZE;
                }
                
                status = PROMPT_STATUS_INSTRUCTION_DECODED;
            }
            else{
                status = PROMPT_STATUS_INVALID;
            }
        } 
        else if(c == KEY_UP){
            if(history_newest != NULL){
                strcpy(buf, history_newest->name);
                strcat(buf, " ");
                strcat(buf, history_newest->arg1);
                strcat(buf, " ");
                strcat(buf, history_newest->arg2);
                buflen = strlen(buf);
            }
           //mvwprintw(prompt->win, 5, 1, "$ teclaarriba");
        }
        else if(c == KEY_DOWN){
            if(history_oldest != NULL){
                strcpy(buf, history_oldest->name);
                strcat(buf, " ");
                strcat(buf, history_oldest->arg1);
                strcat(buf, " ");
                strcat(buf, history_oldest->arg2);
                buflen = strlen(buf);
            }
            //mvwprintw(prompt->win, 5, 1, "$ teclaabajo");
        }
        else if (c == KEY_BACKSPACE || c == 127 || c == 8) {
            if (buflen > 0) {
                buf[buflen - 1] = 0;
                buflen -= 1;
            }
        }
        else if (buflen < 79) {
            buf[buflen] = c;
            buf[buflen + 1] = 0;
            buflen += 1;
        }
        clear_window_part(prompt->win, 5, 1, 1 , 78);
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

int32_t eval(struct instruction *cmd, WINDOW *messages) {
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

int main(void) {
    initscr();
    noecho();
    curs_set(0);

    WINDOW *messages = newwin(10, 80, 0, 0);
    WINDOW *registers = newwin(7, 80, 10, 0);
    struct prompt prompt;
    prompt.win = newwin(7, 80, 17, 0);
    nodelay(prompt.win, TRUE);
    keypad(prompt.win, TRUE);  // Habilitar keypad para la ventana del prompt
    cbreak(); 
    

    box(prompt.win, 0, 0);
    mvwprintw(prompt.win, 0, 35, "|Prompt|");

    char buf[80] = { 0 };
    while (true) {
        instruccions_win(registers);
        messages_win(messages);
        if (prompt_update(&prompt, history) == PROMPT_STATUS_INSTRUCTION_DECODED) {
            eval(prompt.decoded_inst, messages);
            free(prompt.decoded_inst);
        }
        usleep(16E3);
    }
    return 0;
}