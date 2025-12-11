#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Define buffer size for efficient reading
#define BUFSIZE 512

// Struct to hold counts for a single file or cumulative total
struct counts {
    long lines;
    long words;
    long bytes;
    long maxlen; // Holds the longest line length
};

void print_usage() {
    fprintf(2, "Usage: wc [flags] [file ...]\n");
    fprintf(2, "Flags:\n");
    fprintf(2, "  -l  Print the newline counts (lines).\n");
    fprintf(2, "  -w  Print the word counts.\n");
    fprintf(2, "  -c  Print the byte counts.\n");
    fprintf(2, "  -L  Print the longest line length.\n");
    fprintf(2, "If no file is specified, reads from standard input.\n");
    fprintf(2, "If no flags are specified, prints all three counts (l, w, c) by default.\n");
    exit(1);
}

// Function to perform word counting on a file descriptor
// Returns a 'counts' struct with the results for the file.
struct counts wc(int fd, char *name, int print_l, int print_w, int print_c, int print_L)
{
    struct counts result;
    int inword;
    int n;
    char buf[BUFSIZE];
    long current_line_len = 0;

    result.lines = result.words = result.bytes = result.maxlen = 0;
    inword = 0;

    while((n = read(fd, buf, sizeof(buf))) > 0){
        result.bytes += n;
        for(int i = 0; i < n; i++){
            char c = buf[i];
            current_line_len++;

            if(c == '\n') {
                result.lines++;
                // Update maxlen: -1 to exclude the newline character itself
                if (current_line_len - 1 > result.maxlen) {
                    result.maxlen = current_line_len - 1;
                }
                current_line_len = 0; // Reset for the next line
            }

            // Word counting logic: check for common whitespace characters
            // We rely on user/user.h or the compiler's implicit declaration for strchr
            if(strchr(" \r\t\n\v", c)) {
                inword = 0;
            } else if(!inword) {
                result.words++;
                inword = 1;
            }
        }
    }

    // Handle the last line if the file does not end with a newline
    if (current_line_len > 0) {
        if (current_line_len > result.maxlen) {
            result.maxlen = current_line_len;
        }
    }

    if(n < 0){
        fprintf(2, "wc: read error on file %s\n", name);
        // On error, return zero counts for this file
        result.lines = result.words = result.bytes = result.maxlen = 0;
    }

    // Print results for the current file (Skip printing if 'total' is the caller, signified by name being empty)
    if (name[0] != '\0') {
        if (print_l) printf(" %d", (int)result.lines);
        if (print_w) printf(" %d", (int)result.words);
        if (print_c) printf(" %d", (int)result.bytes);
        if (print_L) printf(" %d", (int)result.maxlen);
        printf(" %s\n", name);
    }

    return result;
}

int
main(int argc, char *argv[])
{
    int fd;
    // Initialize all four flags
    int print_l = 0, print_w = 0, print_c = 0, print_L = 0;
    int first_file_index = 1;

    // Variables for Grand Total
    struct counts grand_total = {0, 0, 0, 0};
    int num_files = 0;

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
            else if (*p == 'L') print_L = 1; // Parse -L
            else {
                // Invalid flag error
                fprintf(2, "wc: invalid option -- %c\n", *p);
                print_usage();
            }
        }
        first_file_index = 2; // Flags consumed, files start at index 2
    }

    // Default behavior: If no flags were provided, set the standard three (l, w, c) to true
    if (!print_l && !print_w && !print_c && !print_L) {
        print_l = print_w = print_c = 1;
    }

    // Case 1: No file specified (read from standard input)
    if (argc <= first_file_index) {
        // Read from file descriptor 0 (Standard Input)
        wc(0, "", print_l, print_w, print_c, print_L);
        exit(0);
    }

    // Case 2: One or more files specified
    for (int i = first_file_index; i < argc; i++) {
        struct counts file_counts;
        num_files++;

        if ((fd = open(argv[i], O_RDONLY)) < 0) {
            fprintf(2, "wc: cannot open %s\n", argv[i]);
            continue; // Skip this file and try the next one
        }

        file_counts = wc(fd, argv[i], print_l, print_w, print_c, print_L);
        close(fd);

        // Accumulate totals
        grand_total.lines += file_counts.lines;
        grand_total.words += file_counts.words;
        grand_total.bytes += file_counts.bytes;

        // Update maxlen if the current file has a longer line
        if (file_counts.maxlen > grand_total.maxlen) {
            grand_total.maxlen = file_counts.maxlen;
        }
    }

    // Print Grand Total if multiple files were processed
    if (num_files > 1) {
        if (print_l) printf(" %d", (int)grand_total.lines);
        if (print_w) printf(" %d", (int)grand_total.words);
        if (print_c) printf(" %d", (int)grand_total.bytes);
        if (print_L) printf(" %d", (int)grand_total.maxlen);
        printf(" total\n");
    }

    exit(0);
}
