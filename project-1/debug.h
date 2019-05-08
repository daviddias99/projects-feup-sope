#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>

#define DEBUG

#ifdef DEBUG

  #define print_location() printf("At file %s, function %s, line %d\n", __FILE__, __FUNCTION__, __LINE__);

  #define print_dbg(...) printf(__VA_ARGS__)

#else
  #define print_location()
  #define print_dbg(...) 

#endif



#endif