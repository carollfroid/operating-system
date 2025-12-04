#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define MAX_PATH 512

void print_usage() {
    fprintf(2, "Usage: find <directory> <filename>\n");
    fprintf(2, "Recursively search for <filename> starting from <directory>.\n");
    exit(1);
}

void find_file(char *path, const char *target_name) {
    char buf[MAX_PATH], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0) continue;

        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;

        if (strcmp(de.name, target_name) == 0) {
            strcpy(p, de.name);
            printf("%s\n", buf);
        }

        strcpy(p, de.name);

        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        if (st.type == T_DIR) {
            find_file(buf, target_name);
        }
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3) {
        if (argc == 2 && strcmp(argv[1], "?") == 0) {
            print_usage();
        }

        fprintf(2, "find: requires two arguments: a directory and a filename.\n");
        print_usage();
    }

    find_file(argv[1], argv[2]);

    exit(0);
}
