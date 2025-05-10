#ifndef TRAMPOLINE_HOOK_H
#define TRAMPOLINE_HOOK_H

typedef struct {
    unsigned char* bytes;
    size_t count;
    size_t index;
} Overwrite_bytes;

typedef struct {
    unsigned char* copied_bytes;
    size_t copied_size;

    unsigned char* real_bytes;
    size_t real_size;

    Overwrite_bytes* overwrite_bytes;
    size_t overwrite_count;

} Hook_settings;

void install_trampoline_hook(void* src, void* dst, void** trampoline_function);

#endif //TRAMPOLINE_HOOK_H
