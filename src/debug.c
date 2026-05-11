#ifndef DEBUG_C
#define DEBUG_C

#include <pspkernel.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG_MODE
void save_debug_log(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_list args_copy;
    
    va_start(args, format);
    va_copy(args_copy, args);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    // Also print to stdout for emulator/psplink
    vprintf(format, args_copy);
    printf("\n");
    va_end(args_copy);

    FILE *f = fopen("save_debug.txt", "a");
    if (f) {
        fprintf(f, "%s\n", buffer);
        fclose(f);
    }
}
#else
static inline void save_debug_log(const char* format, ...) { (void)format; }
#endif

#endif // DEBUG_C
