#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

#include "hash_test.h"

// adapted from from the GNU libc manual
// https://www.gnu.org/software/libc/manual/html_node/crypt.html
bool same(char* pass, char* hash)
{
  char *result;
  int ok;

  result = crypt(pass, hash);
  /* Test the result. */
  return strcmp (result, hash) == 0;
}