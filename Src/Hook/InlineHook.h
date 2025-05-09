#ifndef INLINE_HOOK_H
#define INLINE_HOOK_H

#include <windows.h>

#define ARRAY_SIZE(arr) \
        sizeof(arr) / sizeof((arr)[0])

void create_jump_bytes(unsigned char** bytes, void* dst);
void print_error_and_exit();
DWORD change_protections(void* address, size_t size, DWORD new_protections);
void inline_hook(void** src, void** dst);

#endif // INLINE_HOOK_H
