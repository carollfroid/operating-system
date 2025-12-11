#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "proc.h"
#include "types.h"
#include "stat.h"
#include "spinlock.h"

extern struct spinlock wait_lock;

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_getppid(void)
{
struct proc *p = myproc();
  struct proc *parent_p = 0;
  int ppid = 1; // Default for initproc

  // CRITICAL STEP: Acquire the lock that protects p->parent
  acquire(&wait_lock);

  parent_p = p->parent;

  if (parent_p) {
      // If a parent exists, get its PID
      ppid = parent_p->pid;
  }

  // Release the lock immediately after reading the pointer/PID
  release(&wait_lock);

  return ppid;
}

extern void cmostime(struct rtcdate *r); // Extern from time.c

uint64
sys_datetime(void)
{
  uint64 addr; // User space address
  struct rtcdate r;

  // 1. Get the user space address where the structure will be stored
  argaddr(0, &addr);

  // 2. Compute the current time
  cmostime(&r);

  // 3. Copy the 'r' structure (sizeof(r) bytes) to the user-provided address
  if (copyout(myproc()->pagetable, addr, (char*)&r, sizeof(r)) < 0) {
      return -1; // Copy failed
  }

  return 0; // Success
}
extern uint64 getptable(int, uint64);

// This is the function the system call table points to (zero arguments)
uint64
sys_getptable(void)
{
  int nproc;
  uint64 buffer_addr; // The user-provided address

  // Retrieve the first argument (nproc) as an integer
  if (argint(0, &nproc) < 0) return 0; // Failure

  // Retrieve the second argument (buffer_addr) as a memory address
  if (argaddr(1, &buffer_addr) < 0) return 0; // Failure

  // Call the core logic function defined in proc.c
  return getptable(nproc, buffer_addr);
}
uint64
sys_get_avg_metrics(void)
{
    uint64 user_buffer_addr;
    struct avg_metrics metrics;

    // 1. Retrieve the address of the user buffer (the first argument)
    if (argaddr(0, &user_buffer_addr) < 0) return 0;

    // 2. Calculate the averages in the kernel
    if (calculate_metrics(&metrics) < 0) return 0;

    // 3. Copy the result from kernel space (&metrics) to user space
    struct proc *p = myproc();
    if (copyout(p->pagetable, user_buffer_addr, (char *)&metrics,
                sizeof(struct avg_metrics)) < 0) {
        return 0; // Failure
    }

    return 1; // Success (copied one structure)
}
extern int argint(int, int*);

uint64
sys_set_priority_level(void)
{
    int priority_level;
    struct proc *p = myproc();

    // 1. Retrieve the integer argument (the priority level)
    if (argint(0, &priority_level) < 0) return -1;

    // Basic validation (e.g., priority must be 0 or higher)
    if (priority_level < 1) priority_level = 1;
    if (priority_level > 100) priority_level = 100; // Assuming max is 100

    // 2. Safely update the current process's priority field
    acquire(&p->lock);
    p->priority = priority_level;
    release(&p->lock);

    // 3. (Optional but Recommended) If changing priority, yield to allow immediate rescheduling.
    yield();

    return 0; // Success
}
