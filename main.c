#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include "support.h"

int main(int argc, char **argv) {
   uint16_t r0, r1, r2;
   uint16_t imm5, imm_flag;
   uint16_t pc_offset, offset;
   uint16_t cond;
   uint16_t *c;
   char a;
   int i, running;
   enum { PC_START = 0x3000 };

   reg[R_PC] = PC_START;

   if (argc < 2) {
      printf("lc3 [image-file] ...\n");
      exit(EXIT_FAILURE);
   }

   for (i = 0; i < argc; i++) {
      if (!read_image(argv[i])) {
         printf("failed to load image: %s\n", argv[i]);
         exit(EXIT_FAILURE);
      }
   }

   running = 1;
   while (running) {
      uint16_t instr = mem_read(reg[R_PC]++);
      uint16_t op = instr >> 12;

      switch (op) {
         case OP_ADD:
            r0 = (instr >> 9) & 0x7;
            r1 = (instr >> 6) & 0x7;
            imm_flag = (instr >> 5) & 0x1;

            if (imm_flag) {
               imm5 = sign_extend(instr & 0x1f, 5);
               reg[r0] = reg[r1] + imm5;

            } else {
               r2 = instr & 0x7;
               reg[r0] = reg[r1] + reg[r2];
            }

            update_flags(r0);
            break;

         case OP_AND:
            r0 = (instr >> 9) & 0x7;
            r1 = (instr >> 6) & 0x7;
            imm_flag = (instr >> 5) & 0x1;

            if (imm_flag) {
               imm5 = sign_extend(instr & 0x1f, 5);
               reg[r0] = reg[r1] & imm5;

            } else {
               r2 = instr & 0x7;
               reg[r0] = reg[r1] & reg[r2];
            }

            update_flags(r0);
            break;

         case OP_NOT:
            r0 = (instr >> 9) & 0x7;
            r1 = (instr >> 6) & 0x7;
            reg[r0] = ~reg[r1];
            update_flags(r0);
            break;

         case OP_BR:
            cond = (instr >> 9) & 0x7;
            pc_offset = sign_extend(instr & 0x1ff, 9);

            if (cond & reg[R_COND]) {
               reg[R_PC] += pc_offset;
            }

            break;

         case OP_JMP:
            r0 = (instr >> 6) & 0x7;
            reg[R_PC] = reg[r0];
            break;

         case OP_JSR:
            reg[R_R7] = reg[R_PC];

            if ((instr >> 11) ^ 0x1) {
               r0 = (instr >> 6) & 0x7;
               reg[R_PC] = reg[r0];
            } else {
               pc_offset = sign_extend(instr & 0x7ff, 11);
               reg[R_PC] = pc_offset;
            }
            break;

         case OP_LD:
            r0 = (instr >> 9) & 0x7;
            pc_offset = sign_extend(instr & 0x1ff, 9);
            reg[r0] = mem_read(reg[R_PC] + pc_offset);
            update_flags(r0);
            break;

         case OP_LDI:
            r0 = (instr >> 9) & 0x7;
            pc_offset = sign_extend(instr & 0x1ff, 9);
            reg[r0] = mem_read(mem_read(reg[R_PC] + pc_offset));
            update_flags(r0);
            break;

         case OP_LDR:
            r0 = (instr >> 9) & 0x7;
            r1 = (instr >> 6) & 0x7;
            offset = sign_extend(instr & 0x3f, 6);
            update_flags(r0);
            break;

         case OP_LEA:
            r0 = (instr >> 9) & 0x7;
            pc_offset = sign_extend(instr & 0x1ff, 9);
            reg[r0] = reg[R_PC] + pc_offset;
            update_flags(r0);
            break;

         case OP_ST:
            r0 = (instr >> 9) & 0x7;
            pc_offset = sign_extend(instr & 0x1ff, 9);
            mem_write(reg[R_PC] + pc_offset, reg[r0]);
            break;

         case OP_STI:
            r0 = (instr >> 9) & 0x7;
            pc_offset = sign_extend(instr & 0x1ff, 9);
            mem_write(mem_read(reg[R_PC] + pc_offset), reg[r0]);
            break;

         case OP_STR:
            r0 = (instr >> 9) & 0x7;
            r1 = (instr >> 6) & 0x7;
            offset = sign_extend(instr & 0x3f, 6);
            mem_write(reg[r1] + offset, reg[r0]);
            break;

         case OP_TRAP:
            switch(instr & 0xff) {
               case TRAP_GETC:
                  reg[R_R0] = (uint16_t) getchar();
                  break;

               case TRAP_OUT:
                  putc((char) reg[R_R0], stdout);
                  fflush(stdout);
                  break;

               case TRAP_PUTS:
                  c = memory + reg[R_R0];
                  while (*c) {
                     putc((char) *c, stdout);
                     c++;
                  }
                  fflush(stdout);
                  break;

               case TRAP_IN:
                  putc('>', stdout);
                  fflush(stdout);
                  reg[R_R0] = (uint16_t) getchar();
                  putc((char) reg[R_R0], stdout);
                  fflush(stdout);
                  break;

               case TRAP_PUTSP: 
                  c = memory + reg[R_R0];
                  while (*c) {
                     a = (*c) & 0xff;
                     putc(a, stdout);
                     a = (*c) >> 8;
                     if (a) {
                        putc(a, stdout);
                     }
                     c++;
                  }
                  fflush(stdout);
                  break;

               case TRAP_HALT:
                  printf("HALT\n");
                  running = 0;
                  break;

            }

            break;

         case OP_RES:
         case OP_RTI:
         default:
            break;
      }
   }
}
