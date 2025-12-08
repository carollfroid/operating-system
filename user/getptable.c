// user/ps.c

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/proc.h" // Assuming this contains struct proc_info

#define NPROC_MAX 64

// State mapping for display (copy from kernel/proc.c or similar)
char* states[] = {"UNUSED", "USED", "SLEEPING", "RUNNABLE", "RUNNING", "ZOMBIE"};

char* get_state_name(int state_val) {
    if (state_val >= 0 && state_val < 6) {
        return states[state_val];
    }
    return "???";
}

int main(void) {
    struct proc ptable[NPROC_MAX];
    int count;

    // Call the system call
count = getptable(NPROC_MAX, (uint64)ptable);
    if (count == 0) {
        fprintf(2, "ps: Failed to retrieve process table.\n");
        exit(1);
    }

    printf("PID PPID STATE    SIZE NAME\n");// Display output header [cite: 187]

    // Iterate and print the process information
    for (int i = 0; i < count; i++) {
        printf("%d   ", ptable[i].pid);
        printf("%d    ", ptable[i].ppid);
        printf("%-8s ", get_state_name(ptable[i].state));
        printf("%ld   ", ptable[i].sz);
        printf("%s\n", ptable[i].name);
    }

    exit(0);
}
