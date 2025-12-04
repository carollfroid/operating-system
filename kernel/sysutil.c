#include "types.h"
extern int keyboard_int_cnt;
uint64 sys_kbdint()
{
return keyboard_int_cnt;
}
