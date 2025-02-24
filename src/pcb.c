#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>


typedef struct {
    int AX;
    int BX;
    int CX;
    int DX;
    int PC;
    char IR[100];
} PCB;

PCB* createPCB(int AX, int BX, int CX, int DX, int PC, char* IR) {
    PCB* pcb = (PCB*)malloc(sizeof(PCB));
    pcb->AX = AX;
    pcb->BX = BX;
    pcb->CX = CX;
    pcb->DX = DX;
    pcb->PC = PC;
    strcpy(pcb->IR, IR);
    return pcb;
}

void window_pcb (PCB* pcb) {
    printf("AX: %d\n", pcb->AX);
    printf("BX: %d\n", pcb->BX);
    printf("CX: %d\n", pcb->CX);
    printf("DX: %d\n", pcb->DX);
    printf("PC: %d\n", pcb->PC);
    printf("IR: %s\n", pcb->IR);
}








