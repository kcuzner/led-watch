/**
 * General Macro Helpers
 *
 * Kevin Cuzner
 */

#ifndef _MACRO_HELPERS_H_
#define _MACRO_HELPERS_H_

#define NUMARGS(...)  (sizeof((int[]){__VA_ARGS__})/sizeof(int))

#endif //_MACRO_HELPERS_H_

