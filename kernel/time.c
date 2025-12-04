// kernel/time.c

#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "stat.h" // For struct rtcdate
#include "proc.h"
//for date calculations
#define SECS_PER_MIN (60)
#define SECS_PER_HOUR (60 * SECS_PER_MIN)
#define SECS_PER_DAY (24 * SECS_PER_HOUR)
#define SECS_PER_YEAR (365 * SECS_PER_DAY)

#define CLINT 0x2000000
#define CLINT_MTIMECMP 0x4000
#define CLINT_MTIME    0xBFF8
// Define the frequency of the MTIME register (12.5 MHz is standard for QEMU)
// You may need to verify this constant in your specific xv6 version.
#define MTIME_FREQ 12500000

#ifndef BUILD_EPOCH
#define BUILD_EPOCH 0
#endif
// Read the Memory-Mapped Machine Time Register (mtime)
uint64
rdtime()
{
  // Read the 64-bit value from the memory-mapped CLINT region.
  // The address is defined in memlayout.h
  return *(uint64*)(CLINT + CLINT_MTIME);
}

// NOTE: You must also define MTIME_FREQ in a shared header like defs.h or param.h
// if it's not already defined, and ensure its value is correct.

// External constant defined in Makefile:
int is_leap(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
// Days in months, non-leap year (0-indexed)
static int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
void
cmostime(struct rtcdate *r)
{
  uint64 cycles = rdtime();
  uint64 total_seconds = 0;

  // Note: Add 2 hours (7200 seconds) for the time offset requested earlier
  const uint64 TIME_OFFSET_SECS = (2 * 60 * 60);

  // 1. Calculate seconds since Unix Epoch
  total_seconds = cycles / MTIME_FREQ;
  total_seconds += BUILD_EPOCH;
  total_seconds += TIME_OFFSET_SECS;

  // --- Convert Total Seconds to H:M:S ---
  r->second = total_seconds % 60;
  total_seconds /= 60;
  r->minute = total_seconds % 60;
  total_seconds /= 60;
  r->hour = total_seconds % 24;
  total_seconds /= 24; // total_seconds is now total DAYS since epoch

  // --- Convert Total Days to Y/M/D ---
  int year = 1970;
  int day_of_year;

  // Calculate Year
  while (total_seconds >= (day_of_year = (is_leap(year) ? 366 : 365))) {
      total_seconds -= day_of_year;
      year++;
  }
  r->year = year;

  // Calculate Month and Day
  for (int month = 0; month < 12; month++) {
      int days = days_in_month[month];
      // Adjust for February in a leap year
      if (month == 1 && is_leap(year)) {
          days = 29;
      }

      if (total_seconds < days) {
          r->month = month + 1; // Months are 1-indexed
          r->day = total_seconds + 1; // Days are 1-indexed
          break;
      }
      total_seconds -= days;
  }
}
