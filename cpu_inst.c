#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct pcb {
    int32_t ax;
    int32_t bx;
    int32_t cx;
    int32_t dx;
    int32_t pc;
    char ir[128];
};

struct cpu_inst {
    int32_t (*opfn)(int32_t *, int32_t);
    int32_t *ra;
    int32_t *rb;
};

int32_t reg_mov(int32_t *ra, int32_t i) { *ra = i; return 0; }
int32_t reg_add(int32_t *ra, int32_t i) { *ra += i; return 0; }
int32_t reg_sub(int32_t *ra, int32_t i) { *ra -= i; return 0; }
int32_t reg_mul(int32_t *ra, int32_t i) { *ra *= i; return 0; }
int32_t reg_div(int32_t *ra, int32_t i) { *ra /= i; return 0; }
int32_t reg_inc(int32_t *ra, int32_t i) { *ra += 1; return 0; }
int32_t reg_dec(int32_t *ra, int32_t i) { *ra -= 1; return 0; }
int32_t reg_nop(int32_t *ra, int32_t i) { return 1; }

int32_t (*get_operation_from_str_id(char *strid))(int32_t *, int32_t)
{
    if (strncmp(strid, "MOV", 3) == 0) {
        return reg_mov;
    }
    else if (strncmp(strid, "ADD", 3) == 0) {
        return reg_add;
    }
    else if (strncmp(strid, "SUB", 3) == 0) {
        return reg_sub;
    }
    else if (strncmp(strid, "MUL", 3) == 0) {
        return reg_mul;
    }
    else if (strncmp(strid, "DIV", 3) == 0) {
        return reg_div;
    }
    else if (strncmp(strid, "INC", 3) == 0) {
        return reg_inc;
    }
    else if (strncmp(strid, "DEC", 3) == 0) {
        return reg_dec;
    }
    else if (strncmp(strid, "END", 3) == 0) {
        return reg_nop;
    }
    else {
        return NULL;
    }
}

int32_t *
get_register_from_str_id(struct pcb *pcb, char *strid)
{
        if (strncmp(strid, "AX", 2) == 0) {
            return &pcb->ax;
        }
        else if (strncmp(strid, "BX", 2) == 0) {
            return &pcb->bx;
        }
        else if (strncmp(strid, "CX", 2) == 0) {
            return &pcb->cx;
        }
        else if (strncmp(strid, "DX", 2) == 0) {
            return &pcb->dx;
        }
        else if (strncmp(strid, "PC", 2) == 0) {
            return &pcb->pc;
        }

}

struct cpu_inst *
cpu_inst_decode_and_execute(const char *str, struct pcb *pcb)
{
    struct cpu_inst *new_cpu_inst = malloc(sizeof(*new_cpu_inst));
    if (new_cpu_inst) {
        char inst[5] = { 0 }, ra[5] = { 0 }, rb[5] = { 0 };
        char stru[strlen(str) + 1];
        strncpy(stru, str, strlen(str));
        for(size_t i = 0; inst[i] != '\0'; i += 1) {
            inst[i] = toupper(inst[i]);
        }
        sscanf(stru, "%4s %4s %4s", inst, ra, rb);
        new_cpu_inst->opfn = get_operation_from_str_id(inst);
        new_cpu_inst->ra = get_register_from_str_id(pcb, ra);
        new_cpu_inst->rb = get_register_from_str_id(pcb, rb);
        new_cpu_inst->opfn(new_cpu_inst->ra, *new_cpu_inst->rb);
    }

    return NULL;
}
