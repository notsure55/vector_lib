#ifndef INLINEHOOK_H
#define INLINEHOOK_H

#include <windows.h>

#define ARRAY_SIZE(arr) \
        sizeof(arr) / sizeof((arr)[0])

void inline_create_jump_bytes(unsigned char** bytes, const void* dst);
void print_error_and_exit();
DWORD change_protections(const void* address, size_t size, DWORD new_protections);
void inline_hook(void* src, const void* dst);

#endif // INLINEHOOK_H
