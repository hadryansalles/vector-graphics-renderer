#ifndef RVG_UNIT_TEST_H
#define RVG_UNIT_TEST_H

#include <cstdio>
#include <cstdlib>

#define unit_test(condition) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d: failed %s\n", __FILE__, __LINE__, \
                #condition); \
            exit(1); \
        } \
    } while (0)

#endif
