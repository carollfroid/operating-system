#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int pid = getpid();
    int ppid = getppid();

    printf("Parent PID (PPID): %d\n", ppid);
    printf("Current PID (PID): %d\n", pid);

    // Test case: Fork a child process to demonstrate relationship
    if (fork() == 0) {
        // --- CHILD PROCESS ---
        int child_pid = getpid();
        int child_ppid = getppid();

        printf("\n--- Child Process Running ---\n");
        printf("Child PID: %d\n", child_pid);
        printf("Child's PPID: %d (Should match original Parent PID)\n", child_ppid);

        exit(0);
    } else {
        // --- PARENT PROCESS ---
        wait(0); // Parent waits for the child to finish
        printf("\n--- Parent Process Finished ---\n");
    }

    exit(0);
}
