#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Maximum size of the editor's text buffer (arbitrarily large for xv6 simplicity)
#define MAX_BUFFER_SIZE 4096
// Maximum size for a single line buffer (for interactive editing)
#define MAX_LINE_LENGTH 256
#define BACKSPACE 0x08 // ASCII code for backspace

// Global buffer to hold the entire file content in memory
char file_buffer[MAX_BUFFER_SIZE];
int buffer_len = 0;

// Function to handle interactive input and store it in the file_buffer
void read_input() {
    char c;
    char line[MAX_LINE_LENGTH];
    int line_len = 0;

    // Display prompt and instructions
    fprintf(1, "--- XV6 Simple Editor ---\n");
    fprintf(1, "Type text, press ENTER for a new line.\n");
    fprintf(1, "Press Ctrl+D (EOF) to save and exit.\n");
    fprintf(1, "-------------------------\n");

    // Loop to read character by character from standard input (fd 0)
    while (read(0, &c, 1) > 0) {

        // 1. Handle Newline (Enter Key)
        if (c == '\n') {
            // Write the current line buffer to the global file buffer
            if (buffer_len + line_len + 1 < MAX_BUFFER_SIZE) {
                memmove(&file_buffer[buffer_len], line, line_len);
                buffer_len += line_len;
                file_buffer[buffer_len++] = '\n'; // Add the newline character
            } else {
                fprintf(2, "Buffer limit reached.\n");
                break;
            }

            // Echo the newline to the screen
            write(1, "\n", 1);
            line_len = 0; // Reset line buffer

        // 2. Handle Backspace
        } else if (c == BACKSPACE) {
            if (line_len > 0) {
                line_len--;
                // Echo backspace sequence: back, space, back (erases character on screen)
                write(1, "\b \b", 3);
            }

        // 3. Handle Standard Character
        } else if (line_len < MAX_LINE_LENGTH - 1) {
            line[line_len++] = c;
            // Echo character to screen
            write(1, &c, 1);
        } else {
             // Line limit reached (optional: beep or error message)
        }
    }
}

// Function to load existing file content into the buffer
void load_file(int fd) {
    int r;
    r = read(fd, file_buffer, MAX_BUFFER_SIZE - 1);
    if (r > 0) {
        buffer_len = r;
        file_buffer[buffer_len] = 0; // Null-terminate just in case
        fprintf(1, "File loaded (%d bytes). Ready for editing.\n", buffer_len);
    }
    // Close and re-open later for writing, since we cannot lseek.
    close(fd);
}


int
main(int argc, char *argv[])
{
    int fd;
    char *filename;

    if (argc < 2) {
        fprintf(2, "Usage: edit <filename>\n");
        exit(1);
    }
    filename = argv[1];

    // --- 1. Load existing content (O_RDONLY) ---
    if ((fd = open(filename, O_RDONLY)) >= 0) {
        load_file(fd);
    }

    // --- 2. Interactive Input/Editing ---
    // If content was loaded, you would display it here before calling read_input.
    // For simplicity, this version just accepts new input after loading.
    read_input();

    // --- 3. Save (O_CREATE | O_WRONLY | O_TRUNC to overwrite) ---
    // We open the file again with the write/truncate flags.
    if ((fd = open(filename, O_CREATE | O_WRONLY | O_TRUNC)) < 0) {
        fprintf(2, "edit: cannot open/save %s\n", filename);
        exit(1);
    }

    // Write the accumulated content from the buffer
    if (write(fd, file_buffer, buffer_len) != buffer_len) {
        fprintf(2, "edit: write failed.\n");
        close(fd);
        exit(1);
    }

    fprintf(1, "\nFile saved. Exiting.\n");
    close(fd);
    exit(0);
}
