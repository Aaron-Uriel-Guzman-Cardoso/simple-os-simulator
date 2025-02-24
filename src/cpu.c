
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

enum reg { REG_AX, REG_BX, REG_CX, REG_DX, REG_PC, REG_IR, REG_LIMIT};

struct cpu;
int isNumeric(const char *str);
enum reg get_reg_from_str_id(const char *strid);
int32_t (*get_inst_from_str(const char *strid))(struct cpu *, enum reg, enum reg);

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
    int32_t (*instfn)(struct cpu *, enum reg, enum reg);
    enum reg ra, rb;
};

struct inst *
inst_decode(const char *buf)
{
    char name[5], arg1[5], arg2[5];
    sscanf(buf, "%4s %4s %4s", name, arg1, arg2);
    struct inst *new_inst = malloc(sizeof(*new_inst));
    if (new_inst) {
        new_inst->instfn = get_inst_from_str(name);
        new_inst->ra = get_reg_from_str_id(arg1);
        /* Hay que comprobar que los registros son registros o números*/
        if(isNumeric(arg1) == -1 || isNumeric(arg1) == 1){
            return NULL;
        }else{
            //////////implementacion cuando argumento 1 sea numerico o vacio//////////////
        }        
        
        
        if(isNumeric(arg2) == -1){
            ///////implementacion cuando argumento 2 sea vacio//////////////
        }else if(isNumeric(arg2) == 0){
            /////////implementacion cuando argumento 2 sea cadena de caracteres//////////////
            new_inst->rb = get_reg_from_str_id(arg2);
        }else if(isNumeric(arg2) == 1){
            //////////implementacion cuando argumento 2 sea solo numerico//////////////
            
        }
    }
    return new_inst;
}

/*
 * Implementa la verificación de números usando expresiones regulares. 
 */
 int isNumeric(const char *str) {
    if (*str == '\0') return -1; // Cadena vacía regresa -1

    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 0; // Cacteres regresa 0 
        str++;
    }}
    return 1; // Cadena numerica regresa 1
}

int32_t
inst_execute(struct inst *self, struct cpu *cpu)
{
    return self->instfn(cpu, self->ra, self->rb);
}


struct cpu {
    int32_t int_regs[REG_LIMIT];
    FILE *instmem;
};

struct cpu *
cpu_new(void)
{
    struct cpu *new_cpu = calloc(1, sizeof(*new_cpu));
    return new_cpu;
}

int32_t
cpu_mov(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] = rb;
    return 0;
}

int32_t
cpu_add(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] += rb;
    return 0;
}

int32_t
cpu_sub(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] -= rb;
    return 0;
}

int32_t
cpu_mul(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] *= rb;
    return 0;
}

int32_t
cpu_div(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] /= rb;
    return 0;
}

int32_t
cpu_inc(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] += 1;
    return 0;
}

int32_t
cpu_dec(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    self->int_regs[ra] -= 1;
    return 0;
}

int32_t
cpu_nop(struct cpu *self, enum reg ra, enum reg rb)
{
    if (ra >= REG_LIMIT || rb >= REG_LIMIT) {
        return 1;
    }
    return 0;
}

int32_t (*get_inst_from_str(const char *strid))(struct cpu *, enum reg, enum reg)
{
    if (strncmp(strid, "MOV", 3) == 0) {
        return cpu_mov;
    }
    else if (strncmp(strid, "ADD", 3) == 0) {
        return cpu_add;
    }
    else if (strncmp(strid, "SUB", 3) == 0) {
        return cpu_sub;
    }
    else if (strncmp(strid, "MUL", 3) == 0) {
        return cpu_mul;
    }
    else if (strncmp(strid, "DIV", 3) == 0) {
        return cpu_div;
    }
    else if (strncmp(strid, "INC", 3) == 0) {
        return cpu_inc;
    }
    else if (strncmp(strid, "DEC", 3) == 0) {
        return cpu_dec;
    }
    else if (strncmp(strid, "END", 3) == 0) {
        return cpu_nop;
    }
    else {
        return NULL;
    }
}



/*
 * Carga un archivo de instrucciones para ser ejecutada furutamente.
 * Básicamente inicializa un archivo como memoria de ejecución de
 * instrucciones
 */
int32_t
cpu_load_inst(struct cpu *self, const char *filename)
{
    if (!self || !filename) { return -1; }
    self->instmem = fopen(filename, "r");
    if (!self->instmem) { return -1; }
    return 0;
}

/*
 * Ejecuta la siguiente instrucción en la memoria de instrucciones.
 */
int32_t
cpu_next_cycle(struct cpu *self)
{
    if (!self) { return -1; }
    char buf[100];
    fgets(buf, sizeof(buf), self->instmem);
    for (size_t i = 0; buf[i] != '\0'; i += 1) {
        buf[i] = toupper(buf[i]);
    }
    struct inst *inst = inst_decode(buf);
    inst_execute(inst, self);
    return 0;
}