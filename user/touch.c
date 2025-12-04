#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void print_usage() {
    fprintf(2, "Usage: touch <filename>\n");
    fprintf(2, "Creates a new, empty file only if it does not already exist.\n");
    fprintf(2, "Errors if the file exists or creation fails.\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    struct stat st;
    int fd;
    const char *filename;

    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc != 2) {
        fprintf(2, "touch: requires exactly one filename argument.\n");
        print_usage();
    }

    filename = argv[1];

    if (stat(filename, &st) == 0) {
        fprintf(2, "touch: %s already exists. File not created.\n", filename);
        exit(1);
    }

    fd = open(filename, O_CREATE | O_WRONLY);

    if (fd < 0) {
        fprintf(2, "touch: failed to create %s\n", filename);
        exit(1);
    }

    close(fd);

    exit(0);
}
