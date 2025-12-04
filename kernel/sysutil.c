#include "types.h"
#include "defs.h" // Generally required for system call implementations
#include "proc.h" // Generally required for system call implementations

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
