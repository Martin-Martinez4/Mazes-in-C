#include <stdio.h>
#include <stdlib.h>

#define MY_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "\033[31mAssertion failed: %s, file %s, line %d. Message: %s\033[0m\n", \
                    #condition, __FILE__, __LINE__, message); \
        } else{\
          fprintf(stderr, "\033[0;32m%s, file %s, line %d\033[0m\n", \
                    #condition, __FILE__, __LINE__); \
        }\
    } while (0)

int main() {
    int x = 10;
    MY_ASSERT(x == 10, "x should be 10"); 
    MY_ASSERT(x == 11, "x should be 11"); 
    printf("Program continues...\n");
    return 0;
}