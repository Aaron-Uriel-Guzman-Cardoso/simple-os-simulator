#include <ncurses.h>

int main() {
    initscr();           
    cbreak();            
    noecho();          
    refresh();         

    WINDOW *registers = newwin(10, 80, 10, 0);
    box(registers, 0, 0); 

    mvwprintw(registers, 0, 35, "|Registers|");

    mvwprintw(registers, 2, 2, "AX: ");
    mvwprintw(registers, 4, 2, "BX: ");
    mvwprintw(registers, 6, 2, "CX: ");
    mvwprintw(registers, 2, 38, "DX: ");
    mvwprintw(registers, 4, 38, "PC: ");
    mvwprintw(registers, 6, 38, "IR: ");

    wrefresh(registers);

    getch();

    delwin(registers);
    endwin();

    return 0;
}
