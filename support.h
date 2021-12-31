#ifndef HEADER
#define HEADER


uint16_t sign_extend(uint16_t x, int bit_count);
void update_flags(uint16_t r);
int read_image(const char *image_path);
void mem_write(uint16_t address, uint16_t val);
uint16_t mem_read(uint16_t address);

/* memory locations, 65536 total */
uint16_t memory[UINT16_MAX];

/* Registers */
enum {
   R_R0 = 0,
   R_R1,
   R_R2,
   R_R3,
   R_R4,
   R_R5,
   R_R6,
   R_R7,
   R_PC,
   R_COND,
   R_COUNT
};

uint16_t reg[R_COUNT];

/* Memory Mapped Registers */
enum {
   MR_KBSR = 0xfe00, /* keyboard status */
   MR_KBDR = 0xfe02  /* keyboard data */
};

/* Opcodes */
enum {
   OP_BR = 0,
   OP_ADD,
   OP_LD,
   OP_ST,
   OP_JSR,
   OP_AND,
   OP_LDR,
   OP_STR,
   OP_RTI,
   OP_NOT,
   OP_LDI,
   OP_STI,
   OP_JMP,
   OP_RES,
   OP_LEA,
   OP_TRAP
};

/* Flags */
enum {
   FL_POS = 1 << 0,
   FL_ZRO = 1 << 1,
   FL_NEG = 1 << 2
};

/* Trap Codes */
enum {
   TRAP_GETC = 0x20,
   TRAP_OUT = 0x21,
   TRAP_PUTS = 0x22,
   TRAP_IN = 0x23,
   TRAP_PUTSP = 0x24,
   TRAP_HALT = 0x25
};

#endif
