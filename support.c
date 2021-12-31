#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/termios.h>
#include <sys/mman.h>

#include "support.h"

uint16_t sign_extend(uint16_t x, int bit_count);
void update_flags(uint16_t r);
void read_image_file(FILE *file);
int read_image(const char *image_path);
uint16_t swap16(uint16_t x);

/* function: extend x to 16 bits, by preserving sign */
uint16_t sign_extend(uint16_t x, int bit_count) {

   if ((x >> (bit_count - 1)) & 1) {
      x |= (0xFFFF << bit_count);

   }

   return x;
}

/* function: updates the condition register to indicate sign of 
 * contents of register r.
 */
void update_flags(uint16_t r) {

   if (reg[r] == 0) {
      reg[R_COND] = FL_ZRO;

   } else if (reg[r] >> 15) {
      reg[R_COND] = FL_NEG;

   } else {
      reg[R_COND] = FL_POS;

   }

}

/* function: reads machine code and places instructions in memory 
 * origin is given by the first 16 bits of the file.
 * This is where instructions will be placed in memory.
 */
void read_image_file(FILE *file) {
   /* origin is specified by first 16 bits of file */
   uint16_t origin;
   fread(&origin, sizeof(origin), 1, file);
   origin = swap16(origin);


   uint16_t max_read = UINT16_MAX - origin;
   uint16_t *p = memory + origin;
   size_t read = fread(p, sizeof(uint16_t), max_read, file);

   while (read-- > 0) {
      *p = swap16(*p); /* need bytes in little endian */
      p++;

   }

}

/* function: given a valid image_path, calls read_image_file() */
int read_image(const char *image_path) {
   FILE *file = fopen(image_path, "rb");
   if (!file) {
      return 0;
   }

   read_image_file(file);
   fclose(file);
   return 1;
}

/* function: swaps bytes of uint16_t */
uint16_t swap16(uint16_t x) {return (x << 8) | (x >> 8);}

/* function: write val to address */
void mem_write(uint16_t address, uint16_t val) {
   memory[address] = val;
}

/* function: read from address */
uint16_t mem_read(uint16_t address) {

   if (address == MR_KBSR) {
      if (check_key()) {
         memory[MR_KBSR] = (1 << 15);
         memory[MR_KBDR] = getchar();

      } else {
         memory[MR_KBSR] = 0;

      }
   }

   return memory[address];
}

uint16_t check_key() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    return select(1, &readfds, NULL, NULL, &timeout) != 0;
}

struct termios original_tio;

void disable_input_buffering() {
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void handle_interrupt(int signal) {
    restore_input_buffering();
    printf("\n");
    exit(-2);
}
