#ifndef __ASSEMBLER__ // <-- CRITICAL FIX: Only process the following lines if not compiling assembly
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;
typedef unsigned long uint64;

typedef uint64 pde_t;
#endif // __ASSEMBLER__
