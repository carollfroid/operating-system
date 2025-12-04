#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
// #include "string.h" // Removed to fix conflicting declaration errors

#define BUFSIZE 512

void print_usage() {
    fprintf(2, "Usage: wc [flags] [file ...]\n");
    fprintf(2, "Flags:\n");
    fprintf(2, "  -l  Print the newline counts (lines).\n");
    fprintf(2, "  -w  Print the word counts.\n");
    fprintf(2, "  -c  Print the byte counts.\n");
    fprintf(2, "If no file is specified, reads from standard input.\n");
    fprintf(2, "If no flags are specified, prints all three counts.\n");
    exit(1);
}

// Function to perform word counting on a file descriptor
// Uses long for counts to prevent overflow on larger files
void wc(int fd, char *name, int print_l, int print_w, int print_c)
{
    long nlines, nwords, nbytes;
    int inword;
    int n;
    char buf[BUFSIZE];

    nlines = nwords = nbytes = 0;
    inword = 0;

    while((n = read(fd, buf, sizeof(buf))) > 0){
        nbytes += n;
        for(int i = 0; i < n; i++){
            char c = buf[i];

            if(c == '\n') {
                nlines++;
            }

            // Word counting logic using strchr for whitespace detection
            // We rely on user/user.h or the compiler's implicit declaration for strchr
            if(strchr(" \r\t\n\v", c)) {
                inword = 0;
            } else if(!inword) {
                nwords++;
                inword = 1;
            }
        }
    }

    if(n < 0){
        fprintf(2, "wc: read error on file %s\n", name);
        exit(1);
    }

    // Print results based on flags
    // FIX: Using %d with explicit cast for compatibility with xv6 printf
    if (print_l) printf(" %d", (int)nlines);
    if (print_w) printf(" %d", (int)nwords);
    if (print_c) printf(" %d", (int)nbytes);

    // Always print the filename last
    printf(" %s\n", name);
}

int
main(int argc, char *argv[])
{
    int fd;
    int print_l = 0, print_w = 0, print_c = 0;
    int first_file_index = 1;

    // Detailed Check 1: Help Feature
    if (argc > 1 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    // Detailed Check 2: Parse flags if present
    if (argc > 1 && argv[1][0] == '-') {
        char *p = argv[1];
        while (*++p) {
            if (*p == 'l') print_l = 1;
            else if (*p == 'w') print_w = 1;
            else if (*p == 'c') print_c = 1;
            else {
                // Detailed Check 3: Invalid flag error
                fprintf(2, "wc: invalid option -- \n");
                print_usage();
            }
        }
        first_file_index = 2; // Flags consumed, files start at index 2
    }

    // Default behavior: If no flags were provided, set all flags to true
    if (!print_l && !print_w && !print_c) {
        print_l = print_w = print_c = 1;
    }

    // Case 1: No file specified (read from standard input)
    if (argc <= first_file_index) {
        wc(0, "", print_l, print_w, print_c);
        exit(0);
    }

    // Case 2: One or more files specified
    for (int i = first_file_index; i < argc; i++) {
        // Detailed Check 4: File open error
        if ((fd = open(argv[i], O_RDONLY)) < 0) {
            fprintf(2, "wc: cannot open %s\n", argv[i]);
            continue; // Continue to next file
        }
        wc(fd, argv[i], print_l, print_w, print_c);
        close(fd);
    }

    exit(0);
}
