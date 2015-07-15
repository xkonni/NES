#include "nes.h"
/*
 * print error and exit
 */
void print_error(const char *reply) {
  perror(reply);
  exit(1);
}
