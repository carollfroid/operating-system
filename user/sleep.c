#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
void print_usage() {
    // Write to file descriptor 2 (stderr) for error/usage messages
    fprintf(2, "Usage: sleep [number_of_ticks]\n");
    fprintf(2, "Pause execution for the specified number of ticks.\n");
    exit(1); // Exit with a non-zero status to indicate an error/help display
}
int
main(int argc, char *argv[])
{

  int num1 ;
  if (argc != 2) {
        fprintf(2, "sleep: requires exactly one argument.\n");
        print_usage();
    }
    if (strcmp(argv[1], "?") == 0) {
        print_usage();
    }
    num1 = atoi(argv[1]);

    if (num1 <= 0 && strcmp(argv[1], "?") != 0) {
        fprintf(2, "sleep: argument must be a positive integer.\n");
        print_usage();
    }
 sleep(num1);

  exit(0);
}
