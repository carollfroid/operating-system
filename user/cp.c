#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define BUF_SIZE 512

void print_usage() {
    fprintf(2, "Usage: cp <source> <destination>\n");
    fprintf(2, "Copies the content of the source file to the destination file.\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    int fd_src, fd_dst;
    char buf[BUF_SIZE];
    int n;
    const char *source;
    const char *destination;

    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc != 3) {
        fprintf(2, "cp: requires exactly two arguments: source and destination.\n");
        print_usage();
    }

    source = argv[1];
    destination = argv[2];

    if (strcmp(source, destination) == 0) {
        fprintf(2, "cp: '%s' and '%s' are the same file.\n", source, destination);
        exit(1);
    }

    if ((fd_src = open(source, O_RDONLY)) < 0) {
        fprintf(2, "cp: cannot open source file %s\n", source);
        exit(1);
    }

    if ((fd_dst = open(destination, O_WRONLY | O_CREATE | O_TRUNC)) < 0) {
        fprintf(2, "cp: cannot create destination file %s\n", destination);
        close(fd_src);
        exit(1);
    }

    while ((n = read(fd_src, buf, sizeof(buf))) > 0) {
        if (write(fd_dst, buf, n) != n) {
            fprintf(2, "cp: error writing to destination file %s\n", destination);
            close(fd_src);
            close(fd_dst);
            exit(1);
        }
    }

    if (n < 0) {
        fprintf(2, "cp: error reading from source file %s\n", source);
        close(fd_src);
            close(fd_dst);
        exit(1);
    }

    close(fd_src);
    close(fd_dst);

    exit(0);
}
