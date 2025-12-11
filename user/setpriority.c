// user/setpriority.c

#include "kernel/types.h"
#include "user/user.h"

int main() {
    printf("Scheduler switched to Priority-Based.\n");
    set_priority(); // Calls the system call wrapper
    exit(0);
}
