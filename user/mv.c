#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void print_usage() {
    fprintf(2, "Usage: mv <source> <destination>\n");
    fprintf(2, "Moves or renames a file using link and unlink.\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    const char *source;
    const char *destination;

    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc != 3) {
        fprintf(2, "mv: requires exactly two arguments: source and destination.\n");
        print_usage();
    }

    source = argv[1];
    destination = argv[2];

    if (link(source, destination) < 0) {
        fprintf(2, "mv: link failed. Check if source exists or destination is valid.\n");
        exit(1);
    }

    if (unlink(source) < 0) {
        fprintf(2, "mv: unlink failed after successful link. File may exist at both locations.\n");
        // We link successfully but fail to unlink the source. This is a critical error.
        exit(1);
    }

    exit(0);
    //Failure to Move into an Existing Directory
}
