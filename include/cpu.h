#include <stdint.h>

struct cpu;
struct cpu *cpu_new(void);
int32_t cpu_load_inst(struct cpu *self, const char *filename);
int32_t cpu_next_cycle(struct cpu *self);

struct inst;
struct inst *inst_decode(const char *name, const char *arg1, const char *arg2);
int32_t inst_execute(struct inst *self, struct cpu *cpu);


