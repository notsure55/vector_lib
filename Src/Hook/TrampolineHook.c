#include <Hook/TrampolineHook.h>
#include <Hook/InlineHook.h>

#include <windows.h>

#define HOOK_SIZE 15

void trampoline_hook(void* src, void* dst, void** trampoline_function)
{
    // creating jmp_bytes for src function
    unsigned char* jmp_bytes = NULL;
    create_jump_bytes(&jmp_bytes, dst);

    // for storing overwritten bytes
    unsigned char saved_bytes[HOOK_SIZE];

    DWORD old_protect = change_protections(src, HOOK_SIZE, PAGE_EXECUTE_READWRITE);

    // save bytes
    memcpy(saved_bytes, src, HOOK_SIZE);
    // overwrite bytes with jmp
    memcpy(src, jmp_bytes, 12);

    FlushInstructionCache(GetCurrentProcess(), src, HOOK_SIZE);

    printf("Hook installed at: %zx\n", (size_t)src);

    change_protections(src, HOOK_SIZE, old_protect);

    *trampoline_function = VirtualAlloc(NULL, HOOK_SIZE + 12, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    memcpy(*trampoline_function, saved_bytes, HOOK_SIZE);

    unsigned char* jmp_to_src_bytes = NULL;
    create_jump_bytes(&jmp_to_src_bytes, (void*)(((size_t)src) + HOOK_SIZE));

    size_t trampoline_address = ((size_t)(*trampoline_function)) + HOOK_SIZE;

    // write jmp back to src function, to resume execution
    memcpy((void*)trampoline_address, jmp_to_src_bytes, 12);

    printf("Trampoline Setuph at: %zx\n", trampoline_address - HOOK_SIZE);

    free(jmp_bytes);
    free(jmp_to_src_bytes);
}
