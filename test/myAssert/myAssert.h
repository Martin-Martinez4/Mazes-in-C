#ifndef H_TESTS_MY_ASSERT
#define H_TESTS_MY_ASSERT

#define MY_ASSERT(condition, message)                                                              \
  do {                                                                                             \
    if (!(condition)) {                                                                            \
      fprintf(                                                                                     \
          stderr,                                                                                  \
          "=====\n\033[31mAssertion failed: %s, file %s, line %d. Message: %s\033[0m\n=====\n",    \
          #condition, __FILE__, __LINE__, message);                                                \
    } else {                                                                                       \
      fprintf(stderr, "=====\n\033[0;32m%s, file %s, line %d\033[0m\n=====\n", #condition,         \
              __FILE__, __LINE__);                                                                 \
    }                                                                                              \
  } while (0)

#endif