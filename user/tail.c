#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

#define MAX_LINES 1024  // Maximum number of lines tail can track
#define BUF_SIZE 512    // Buffer size for reading file chunks
#define DEFAULT_N 10    // Default number of lines to print

// Function to display the usage/help message
void print_usage() {
    fprintf(2, "Usage: tail [-n <number>] [file]\n");
    fprintf(2, "Displays the last N lines of a file or standard input.\n");
    fprintf(2, "Default N is 10.\n");
    exit(1);
}

// Function to print the last N lines (using read only, NO LSEEK)
void tail(char *filename, int n_lines) {
    int offsets[MAX_LINES + 1];
    char buf[BUF_SIZE];
    int current_offset = 0;
    int line_count = 0;
    int bytes_read;
    int fd; // Local file descriptor

    // File must be reopened for the second pass, so stdin is not supported in this mode
    // If filename is null, it means stdin was requested, which is not seekable.
    if (filename == 0) {
        fprintf(2, "tail: reading from stdin is not supported without lseek.\n");
        exit(1);
    }

    // Open the file for the first pass (reading offsets)
    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(2, "tail: cannot open %s for first pass.\n", filename);
        exit(1);
    }

    // Pass 1: Read file and record line offsets
    offsets[0] = 0;
    while ((bytes_read = read(fd, buf, BUF_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buf[i] == '\n') {
                line_count++;
                if (line_count < MAX_LINES) {
                    offsets[line_count] = current_offset + i + 1;
                }
            }
        }
        current_offset += bytes_read;
    }

    if (bytes_read < 0) {
        fprintf(2, "tail: read error during first pass.\n");
        close(fd);
        exit(1);
    }

    close(fd); // Close the file after the first pass

    // 2. Determine start index and byte offset
    int start_line_index;

    if (line_count <= n_lines) {
        start_line_index = 0;
    } else {
        start_line_index = line_count - n_lines;
        if (start_line_index < 0) {
             start_line_index = 0;
        }
    }

    int array_index_to_use = start_line_index;
    if (array_index_to_use > MAX_LINES) {
        array_index_to_use = MAX_LINES;
    }

    // Get the byte offset where printing should start
    int start_byte_offset = 0;
    if (line_count > MAX_LINES) {
        start_byte_offset = offsets[MAX_LINES];
    } else {
        start_byte_offset = offsets[array_index_to_use];
    }

    // Pass 2: Open the file again (pointer reset to 0)
    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(2, "tail: cannot open %s for second pass.\n", filename);
        exit(1);
    }

    // Pass 3: Read and discard data until the start offset is reached
    int bytes_to_discard = start_byte_offset;
    int bytes_discarded = 0;

    while (bytes_discarded < bytes_to_discard) {
        int to_read = bytes_to_discard - bytes_discarded;
        if (to_read > BUF_SIZE) to_read = BUF_SIZE;

        bytes_read = read(fd, buf, to_read);
        if (bytes_read <= 0) {
            fprintf(2, "tail: failed to discard to start offset.\n");
            close(fd);
            exit(1);
        }
        bytes_discarded += bytes_read;
    }

    // Pass 4: Read and print the remaining content
    while ((bytes_read = read(fd, buf, BUF_SIZE)) > 0) {
        write(1, buf, bytes_read);
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
    int n_lines = DEFAULT_N;
    char *filename = 0;
    int i = 1;

    // --- 1. DETAILED CHECKS and Flag/Argument Parsing ---

    if (argc > 1 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc > 1 && strcmp(argv[i], "-n") == 0) {
        i++;
        if (i >= argc) {
            fprintf(2, "tail: -n requires a number argument.\n");
            print_usage();
        }
        n_lines = atoi(argv[i]);
        if (n_lines <= 0) {
            fprintf(2, "tail: number of lines must be a positive integer.\n");
            exit(1);
        }
        i++;
    }

    if (i < argc) {
        filename = argv[i];
    }

    // --- 2. Core Logic Execution ---

    // We pass the filename string instead of the descriptor because we need to reopen it (no lseek)
    tail(filename, n_lines);

    exit(0);

    //works but needs to be tested
}
