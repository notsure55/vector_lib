#include <Hook/InlineHook.h>

#include <stdio.h>

#define INLINE_BYTES_SIZE 12

void inline_create_jump_bytes(unsigned char** bytes, const void* dst)
{                                                // mov rax, address
    unsigned char jmp_bytes[INLINE_BYTES_SIZE] = {0x48, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC,
                                                  0xFF, 0xE0}; //jmp rax

    const size_t address = (size_t)dst;
    // replace address in byte array
    memcpy(&jmp_bytes[2], &address, sizeof(size_t));

    *bytes = (unsigned char*)malloc(INLINE_BYTES_SIZE * sizeof(unsigned char));
    memcpy(*bytes, jmp_bytes, INLINE_BYTES_SIZE);
}

void print_error_and_exit()
{
    char error_string[512];

    DWORD message_id = GetLastError();

    FormatMessage(
          FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
          NULL,
          message_id,
          0,
          error_string,
          sizeof(error_string),
          NULL
    );

    fprintf(stderr, "ERROR: %s", error_string);

    exit(EXIT_FAILURE);
}

DWORD change_protections(const void* address, size_t size, DWORD new_protections)
{
    DWORD old_protect;

    if (VirtualProtect(address, size, new_protections, &old_protect) == 0)
    {
        print_error_and_exit();
    }

    return old_protect;
}

void inline_hook(void* src, const void* dst)
{
    unsigned char* jmp_bytes = NULL;
    inline_create_jump_bytes(&jmp_bytes, dst);

    // change protections so we can write our jmp bytes to function prelude
    const DWORD old_protect = change_protections(src, INLINE_BYTES_SIZE, PAGE_EXECUTE_READWRITE);

    // copy over bytes
    memcpy(src, jmp_bytes, INLINE_BYTES_SIZE);

    // change protections back
    change_protections(src, INLINE_BYTES_SIZE, old_protect);

    printf("Hook successfully installed at 0x%zx\n", (size_t)src);
}
