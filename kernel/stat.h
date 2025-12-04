#ifndef __KERNEL_STAT_H__ // <-- 1. START of the guard
#define __KERNEL_STAT_H__ // <-- 2. Define the guard macro


#define T_DIR     1   // Directory
#define T_FILE    2   // File
#define T_DEVICE  3   // Device

struct stat {
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short type;  // Type of file
  short nlink; // Number of links to file
  uint64 size; // Size of file in bytes
};
// kernel/stat.h (or time.h)

struct rtcdate {
  int year;
  int month;
  int day;
  int hour;
  int minute;
  int second;
};
#endif // __KERNEL_STAT_H__ // <-- 3. END of the guard
