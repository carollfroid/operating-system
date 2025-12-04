#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void print_usage() {
    fprintf(2, "Usage: fact <integer>\n");
    fprintf(2, "Calculates the factorial of a non-negative integer (0-12).\n");
    exit(1);
}

// Factorial function using iteration (long long is used to prevent overflow for larger inputs)
long long factorial(int n) {
    if (n < 0) return -1;
    if (n == 0) return 1;

    long long result = 1;
    for (int i = 1; i <= n; i++) {
        // Simple overflow check: Factorial grows very fast.
        // 13! exceeds the capacity of a standard 32-bit int.
        // We stop at 20! for long long, but xv6's printf likely uses standard integer formats.
        // In a strict xv6 32-bit environment, 13! overflows, so we handle it gracefully.
        if (result > 2147483647 / i) { // 2^31 - 1 (max signed int)
             return -2; // Custom code for overflow error
        }
        result *= i;
    }
    return result;
}

int
main(int argc, char *argv[])
{
    int n;
    long long result;

    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc != 2) {
        fprintf(2, "fact: requires exactly one non-negative integer argument.\n");
        print_usage();
    }

    // Check 1: Conversion and non-negative requirement
    n = atoi(argv[1]);

    if (n < 0) {
        fprintf(2, "fact: argument must be a non-negative integer.\n");
        exit(1);
    }

    // Check 2: Max calculation limit (to prevent immediate overflow in the small xv6 int type)
    if (n > 12) {
        fprintf(2, "fact: input exceeds practical limit for 32-bit integer (max is 12).\n");
        exit(1);
    }

    result = factorial(n);

    if (result == -2) {
        fprintf(2, "fact: calculation resulted in integer overflow.\n");
        exit(1);
    }

    printf("%d! = %d\n", n, (int)result);

    exit(0);
}
