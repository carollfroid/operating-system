// user/rand.c

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  printf("Random number 1: %d\n", urand());
  printf("Random number 2: %d\n", urand());
  printf("Random number 3: %d\n", urand());

  exit(0);
}
