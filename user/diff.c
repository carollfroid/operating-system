#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"
// #include "string.h" // REMOVED to fix conflicting declaration errors

#define MAX_LINE_LEN 256
#define BUF_SIZE 512

// Helper function to read one line from a file descriptor
// Returns the length of the line read (including the newline), or -1 on EOF/Error.
int read_line(int fd, char *buffer, int max_len) {
    int i = 0;
    char c;

    // Read character by character until newline, EOF, or buffer full
    while (i < max_len - 1 && read(fd, &c, 1) > 0) {
        buffer[i++] = c;
        if (c == '\n') {
            break;
        }
    }

    if (i == 0) {
        return -1; // EOF or Error
    }

    // Null-terminate the string
    buffer[i] = '\0';
    return i;
}

// Function to display the usage/help message
void print_usage() {
    fprintf(2, "Usage: diff <file1> <file2>\n");
    fprintf(2, "Compares two files line by line and reports differences.\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    int fd1, fd2;
    char line1[MAX_LINE_LEN];
    char line2[MAX_LINE_LEN];
    int len1, len2;
    int line_num = 0;
    int identical = 1;

    // --- 1. DETAILED CHECKS and Flag/Argument Parsing ---

    if (argc == 2 && strcmp(argv[1], "?") == 0) {
        print_usage();
    }

    if (argc != 3) {
        fprintf(2, "diff: requires exactly two filename arguments.\n");
        print_usage();
    }

    // Check 2: Open file 1
    if ((fd1 = open(argv[1], O_RDONLY)) < 0) {
        fprintf(2, "diff: cannot open %s\n", argv[1]);
        exit(1);
    }

    // Check 3: Open file 2
    if ((fd2 = open(argv[2], O_RDONLY)) < 0) {
        fprintf(2, "diff: cannot open %s\n", argv[2]);
        close(fd1);
        exit(1);
    }

    // --- 2. Core Comparison Logic ---

    while (1) {
        line_num++;

        // Read lines from both files simultaneously
        len1 = read_line(fd1, line1, MAX_LINE_LEN);
        len2 = read_line(fd2, line2, MAX_LINE_LEN);

        // Case A: Both files are still reading content (not EOF)
        if (len1 > 0 && len2 > 0) {
            // Check if lines differ
            if (strcmp(line1, line2) != 0) {
                identical = 0;
                printf("Line %d differs:\n", line_num);
                printf("< %s", line1); // line1 already includes \n
                printf("> %s", line2); // line2 already includes \n
            }

        // Case B: File 1 reached EOF, but File 2 still has content
        } else if (len1 == -1 && len2 > 0) {
            identical = 0;
            printf("Line %d only in %s:\n", line_num, argv[2]);
            printf("> %s", line2);

            // Continue reading remaining lines from file 2 until EOF
            while ((len2 = read_line(fd2, line2, MAX_LINE_LEN)) > 0) {
                line_num++;
                printf("Line %d only in %s:\n", line_num, argv[2]);
                printf("> %s", line2);
            }
            break; // Finished reading file 2

        // Case C: File 2 reached EOF, but File 1 still has content
        } else if (len1 > 0 && len2 == -1) {
            identical = 0;
            printf("Line %d only in %s:\n", line_num, argv[1]);
            printf("< %s", line1);

            // Continue reading remaining lines from file 1 until EOF
            while ((len1 = read_line(fd1, line1, MAX_LINE_LEN)) > 0) {
                line_num++;
                printf("Line %d only in %s:\n", line_num, argv[1]);
                printf("< %s", line1);
            }
            break; // Finished reading file 1

        // Case D: Both files reached EOF simultaneously
        } else if (len1 == -1 && len2 == -1) {
            break;
        } else {
            // Should not happen, but serves as a safety break
            break;
        }
    }

    // --- 3. Final Output ---

    if (identical) {
        printf("Files are identical\n");
    }

    close(fd1);
    close(fd2);

    exit(0);
}
// NEEDSS TESTINGG
