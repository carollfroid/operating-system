#ifndef __SPINLOCK_H__ // Standard include guard
#define __SPINLOCK_H__

typedef unsigned int uint;
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
#endif // __SPINLOCK_H__
