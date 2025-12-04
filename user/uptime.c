// user/uptimetest.c

#include "kernel/types.h"
#include "user/user.h"

int main(void)
{
  int ticks_a = uptime();
  printf("Uptime (Initial): %d ticks\n", ticks_a);

  // Sleep for a short period (10 ticks)
  sleep(10);

  int ticks_b = uptime();
  printf("Uptime (After 10ms sleep): %d ticks\n", ticks_b);

  // The difference should be roughly 10 (plus a few extra for process overhead)
  printf("Ticks elapsed during sleep: %d\n", ticks_b - ticks_a);

  exit(0);
}
