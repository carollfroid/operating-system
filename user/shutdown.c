// user/shutdown.c

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h" // Includes the declaration for shutdown()

int main(int argc, char *argv[]) {
    // Check if the user passed any unnecessary arguments
    if (argc > 1) {
        fprintf(2, "Usage: shutdown\n");
        exit(1);
    }

    // Call the user-space wrapper function.
    // This function transitions to the kernel and executes sys_shutdown.
    shutdown();

    // This line should never be reached if the kernel sys_shutdown
    // successfully halts QEMU.
    fprintf(2, "Shutdown call failed.\n");
    exit(0);
}
