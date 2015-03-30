#include <stdlib.h>
#include <string.h>
#include <stdio.h>


int main(int ac, char *av[])
{
  if (ac != 4) {
    fprintf(stderr, "usage : %s start num_line width\n", av[0]);
    return 1;
  }

  size_t start    = strtoul(av[1], 0, 10);
  size_t num_line = strtoul(av[2], 0, 10);
  size_t width    = strtoul(av[3], 0, 10);

  char * s = malloc(width + 1);
  if (!s)
    return 1;
  for (size_t i = 0; i < width; ++i) {
    s[i] = 'x';
  }
  s[width] = '\0';

  for (size_t l = start; l < start+num_line; ++l) {
    fprintf(stdout, "%lu %.*s\n", l, width, s);
  }

  return 0;
}
