// user/setfcfs.c

#include "kernel/types.h"
#include "user/user.h"

int main() {
    printf("Scheduler switched to First Come First Serve (FCFS).\n");
    set_fcfs(); // Calls the system call wrapper
    exit(0);
}
