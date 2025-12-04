// user/timetest.c

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int main(void)
{
  struct rtcdate r;

  if (datetime(&r) < 0) {
    printf("datetime system call failed\n");
    exit(1);
  }

  printf("Year: %d, Month: %d, Day: %d\n", r.year, r.month, r.day);
  printf("Time: %d:%d:%d\n", r.hour, r.minute, r.second);

  exit(0);
}
