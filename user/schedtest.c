
#include "kernel/types.h"
#include "user/user.h"

// Re-define structure if not included via a header
struct avg_metrics {
    uint avg_turnaround;
    uint avg_waiting;
};

// --- Workload Function ---
void run_test_workload(int num_children, int is_priority_test) {
    int i;
    printf("  Forking %d children to perform work...\n", num_children);

    for (i = 0; i < num_children; i++) {
        if (fork() == 0) {
            // Child process: Consume CPU time. This is the "work."
            // The longer this loop, the clearer the scheduling difference.
            if (is_priority_test) {
                // Set descending priorities: First child (i=0) gets highest priority (1),
                // last child (i=4) gets lowest priority (5).
                int priority_level = num_children - i;
                set_priority_level(priority_level);
                printf("  Child %d (PID %d) set priority to %d.\n", i + 1, getpid(), priority_level);
            }
            for (long j = 0; j < 500000; j++) {}
            exit(0);
        }
    }

    // Parent waits for all children to complete
    for (i = 0; i < num_children; i++) {
        wait(0);
    }
    printf("  Workload complete. All children terminated.\n");
}

// --- Reporting Function ---
void report_metrics(char *scheduler_name) {
    struct avg_metrics results;

    // Call the kernel system call using the address of the local 'results' struct
    if (get_avg_metrics((uint64)&results) == 1) {
        printf("\n=== %s RESULTS ===\n", scheduler_name);
        printf("Avg Turnaround Time: %d ticks\n", results.avg_turnaround);
        printf("Avg Waiting Time: %d ticks\n", results.avg_waiting);
    } else {
        printf("Error: Kernel failed to retrieve metrics for %s.\n", scheduler_name);
    }
}

// --- Main Test Function ---
int main(int argc, char *argv[]) {
    int num_children = 5;

    // 1. Test FCFS Scheduling
    printf("\n[TEST 1] Switching to FCFS mode.\n");
    set_fcfs();
    run_test_workload(num_children,0);
    report_metrics("FCFS");

    // 2. Test Priority Scheduling
    printf("\n[TEST 2] Switching to PRIORITY mode.\n");
    set_priority();
    // A separate workload could be used here to set specific priorities if needed
    run_test_workload(num_children, 1);
    report_metrics("PRIORITY");

    printf("\nScheduler Benchmark Finished.\n");
    exit(0);
}
