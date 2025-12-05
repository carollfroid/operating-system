#include "types.h"
#include "defs.h" // Generally required for system call implementations
#include "proc.h" // Generally required for system call implementations
#include "spinlock.h" // Needed for struct spinlock definition
// Existing implementation for kbdint:
extern int keyboard_int_cnt;
uint64 sys_kbdint()
{
    return keyboard_int_cnt;
}

// -----------------------------------------------------------------
// Implementation for countsyscall

// Global counter for total system calls (defined here in the implementation file)
uint64 syscall_count = 0; // <-- 1. DECLARE & INITIALIZE

// Implementation of the countsyscall system call
uint64 sys_countsyscall(void)
{
    // Return the total count
    return syscall_count; // <-- 2. RETURN THE COUNTER VALUE
}
#define FINISHER_ADDR 0x100000

uint64 sys_shutdown(void) {
    printf("XV6: System shutting down via MMIO...\n");

    // Declare a volatile pointer to the Finisher MMIO address
    volatile uint32 *finisher = (volatile uint32 *)FINISHER_ADDR;

    // Use a clean write, often 0x5555 for guaranteed exit success.
    *finisher = 0x5555; // <--- Use a robust magic value

    // Crucial: The CPU needs to halt immediately to prevent the function from returning.
    // If QEMU fails to recognize the signal, the system must freeze here.
    for(;;) {
        // If your xv6 version has a simple 'wfi()' function, use that.
        // Otherwise, use the asm volatile instruction.
        asm volatile("wfi");
    }

    return 0; // Unreachable
}
