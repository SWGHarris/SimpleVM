/* function: extend x to 16 bits, by preserving sign */
uint16_t sign_extend(uint16_t x, int but_count) {

   if ((x >> (bit_count - 1)) & 1) {
      x |= (0xFFFF << bit_count);

   }

   return x;
}

void update_flags(uint16_t r) {

   if (reg[r] == 0) {
      reg[R_COND] = FL_ZRO;

   } else if (reg[r] >> 15) {
      reg[R_COND] = FL_NEG;

   } else {
      reg[R_COND] = FL_POS;

   }

}

void read_image_file(FILE *file) {
   /* origin is specified by forst 16 bits of file */
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

/* Function: swaps bytes of uint16_t */
uint16_t swap16(uint16_t x) {return (x << 8) | (x >> 8);}
