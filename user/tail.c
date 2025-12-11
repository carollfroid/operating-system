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
    // Use a pointer and dynamic allocation for the offsets array
    int *offsets;
    char buf[BUF_SIZE];
    int file_size = 0; // Tracks total bytes read (absolute offset)
    int line_count = 0; // Total lines (terminated or not)
    int bytes_read;
    int fd;

    if (filename == 0) {
        fprintf(2, "tail: reading from stdin is not supported without lseek.\n");
        exit(1);
    }

    // Allocate offsets array on the heap (MAX_LINES+1 spots: offsets[0] to offsets[MAX_LINES])
    offsets = (int*)malloc((MAX_LINES + 1) * sizeof(int));
    if (offsets == 0) {
        fprintf(2, "tail: malloc failed.\n");
        exit(1);
    }

    // --- Pass 1: Read file and record line offsets ---
    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(2, "tail: cannot open %s for first pass.\n", filename);
        free(offsets);
        exit(1);
    }

    offsets[0] = 0; // The start of the first line (Line 1) is at byte 0.

    while ((bytes_read = read(fd, buf, BUF_SIZE)) > 0) {
        for (int i = 0; i < bytes_read; i++) {
            if (buf[i] == '\n') {
                line_count++; // Total terminated lines found (N)

                // Store the offset for the *next* line (Line N+1) at array index N.
                if (line_count <= MAX_LINES) {
                    offsets[line_count] = file_size + i + 1;
                }
            }
        }
        file_size += bytes_read;
    }

    if (bytes_read < 0) {
        fprintf(2, "tail: read error during first pass.\n");
        close(fd);
        free(offsets);
        exit(1);
    }

    close(fd); // Close the file after the first pass

    // --- Critical Fix: Handle Uncounted Final Line ---
    if (file_size > 0 && line_count < MAX_LINES) {
        // If the file is not empty AND the last recorded offset (offsets[line_count])
        // is less than the total file size, a non-terminated line exists.
        if (line_count == 0 || offsets[line_count] < file_size) {
            line_count++; // Increment count to include the final segment.
        }
    }


    // --- Pass 2: Determine start offset ---

    int start_index_for_offsets;

    if (line_count <= n_lines) {
        // Start from the very beginning (offsets[0]).
        start_index_for_offsets = 0;
    } else {
        // Correct formula: Index = Total Lines - Lines to Show.
        // This index points to the start of the Nth-to-last line segment.
        start_index_for_offsets = line_count - n_lines;
    }

    // Cap the index if the file was too long (more than MAX_LINES lines were found).
    if (start_index_for_offsets > MAX_LINES) {
        start_index_for_offsets = MAX_LINES;
    }

    int start_byte_offset = offsets[start_index_for_offsets];


    // --- Pass 3: Open file again and discard bytes ---
    if ((fd = open(filename, O_RDONLY)) < 0) {
        fprintf(2, "tail: cannot open %s for second pass.\n", filename);
        free(offsets);
        exit(1);
    }

    int bytes_to_discard = start_byte_offset;
    int bytes_discarded = 0;

    while (bytes_discarded < bytes_to_discard) {
        int to_read = bytes_to_discard - bytes_discarded;
        if (to_read > BUF_SIZE) to_read = BUF_SIZE;

        bytes_read = read(fd, buf, to_read);
        if (bytes_read <= 0) {
            fprintf(2, "tail: failed to discard to start offset.\n");
            close(fd);
            free(offsets);
            exit(1);
        }
        bytes_discarded += bytes_read;
    }

    // --- Pass 4: Read and print the remaining content ---
    while ((bytes_read = read(fd, buf, BUF_SIZE)) > 0) {
        write(1, buf, bytes_read);
    }

    // Final cleanup
    close(fd);
    free(offsets);
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

    // Parse the -n flag and its argument
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

    // Get the optional filename
    if (i < argc) {
        filename = argv[i];
    }

    // --- 2. Core Logic Execution ---
    tail(filename, n_lines);

    exit(0);
}
