#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../include/cpu_inst.h"

struct pcb {
    int32_t registers[REG_LIMIT];
};

struct pcb *
pcb_new(void)
{
    struct pcb *new_pcb = calloc(1, sizeof(*new_pcb));
    return new_pcb;
}

int32_t
pcb_mov(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] = rb;
    return 0;
}

int32_t
pcb_add(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] += rb;
    return 0;
}

int32_t
pcb_sub(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] -= rb;
    return 0;
}

int32_t
pcb_mul(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] *= rb;
    return 0;
}

int32_t
pcb_div(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] /= rb;
    return 0;
}

int32_t
pcb_inc(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] += 1;
    return 0;
}

int32_t
pcb_dec(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->registers[ra] -= 1;
    return 0;
}

int32_t
pcb_nop(struct pcb *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    return 0;
}


int32_t (*get_inst_from_str(const char *strid))(struct pcb *, enum reg, enum reg)
{
    if (strncmp(strid, "MOV", 3) == 0) {
        return pcb_mov;
    }
    else if (strncmp(strid, "ADD", 3) == 0) {
        return pcb_add;
    }
    else if (strncmp(strid, "SUB", 3) == 0) {
        return pcb_sub;
    }
    else if (strncmp(strid, "MUL", 3) == 0) {
        return pcb_mul;
    }
    else if (strncmp(strid, "DIV", 3) == 0) {
        return pcb_div;
    }
    else if (strncmp(strid, "INC", 3) == 0) {
        return pcb_inc;
    }
    else if (strncmp(strid, "DEC", 3) == 0) {
        return pcb_dec;
    }
    else if (strncmp(strid, "END", 3) == 0) {
        return pcb_nop;
    }
    else {
        return NULL;
    }
}

enum reg
get_reg_from_str_id(const char *strid)
{
    if (strncmp(strid, "AX", 2) == 0) {
        return REG_AX;
    }
    else if (strncmp(strid, "BX", 2) == 0) {
        return REG_BX;
    }
    else if (strncmp(strid, "CX", 2) == 0) {
        return REG_CX;
    }
    else if (strncmp(strid, "DX", 2) == 0) {
        return REG_DX;
    }
    else if (strncmp(strid, "PC", 2) == 0) {
        return REG_PC;
    }
    else if (strncmp(strid, "IR", 2) == 0) {
        return REG_IR;
    }
    else {
        /*
         * Regresamos un registro que no existe cuando no hay conversión
         * válida. 
         */
        return REG_LIMIT;
    }

}

struct inst {
    int32_t (*instfn)(struct pcb *, enum reg, enum reg);
    enum reg ra, rb;
};

struct inst *
inst_decode(const char *name, const char *arg1, const char *arg2)
{
    struct inst *new_inst = malloc(sizeof(*new_inst));
    if (new_inst) {
        new_inst->instfn = get_inst_from_str(name);
        new_inst->ra = get_reg_from_str_id(arg1);
        new_inst->rb = get_reg_from_str_id(arg2);
    }
    return new_inst;
}

int32_t
inst_execute(struct inst *self, struct cpu *cpu)
{
    return self->instfn(cpu, self->ra, self->rb);
}
