#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void print_usage() {
    fprintf(2, "Usage: add <number1> <number2>\n");
    fprintf(2, "Calculates the sum of two integers.\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    int num1, num2, sum;

    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc != 3) {
        fprintf(2, "add: requires exactly two integer arguments.\n");
        print_usage();
    }

    num1 = atoi(argv[1]);
    num2 = atoi(argv[2]);

    sum = num1 + num2;
    printf("sum = %d\n", sum);

    exit(0);
}
