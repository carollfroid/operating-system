
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int count_before = countsyscall();
  printf("Count before running getpid: %d\n", count_before);

  // Call a few system calls to increment the counter
  int pid = getpid();
  int count_after = countsyscall();

  printf("Current PID is: %d\n", pid);
  printf("Count after getpid and countsyscall: %d\n", count_after);

  // The difference should be 2 (one for getpid, one for the final countsyscall)
  printf("New calls invoked: %d\n", count_after - count_before);

  exit(0);
}
