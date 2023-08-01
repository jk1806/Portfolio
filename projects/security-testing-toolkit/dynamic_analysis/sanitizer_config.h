#ifndef SANITIZER_CONFIG_H
#define SANITIZER_CONFIG_H

/* AddressSanitizer options */
#define ASAN_OPTIONS "detect_leaks=1:abort_on_error=1:check_initialization_order=1"

/* UndefinedBehaviorSanitizer options */
#define UBSAN_OPTIONS "print_stacktrace=1:abort_on_error=1"

/* MemorySanitizer options */
#define MSAN_OPTIONS "print_stats=1:abort_on_error=1"

/* ThreadSanitizer options */
#define TSAN_OPTIONS "halt_on_error=1:abort_on_error=1"

#endif /* SANITIZER_CONFIG_H */

