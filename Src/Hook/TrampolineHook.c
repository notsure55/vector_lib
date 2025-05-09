#include <Hook/TrampolineHook.h>
#include <Hook/InlineHook.h>
#include <stdbool.h>
#include <stdint.h>

#include <windows.h>

#define HOOK_SIZE 15
#define JMP_SIZE 12

bool resolve_func_address(void** address)
{
    // https://www.felixcloutier.com/x86/jmp
    // 5 bytes for Jump near, relative, RIP = RIP + 32-bit displacement sign extended to 64-bits.
    DWORD old_protect = change_protections(*address, 5, PAGE_EXECUTE_READ);

    // save jmp instruction
    unsigned char bytes[5];
    memcpy(bytes, *address, 5);

    if (bytes[0] == 0xE9) {
        uint32_t relative_offset;
        memcpy(&relative_offset, &bytes[1], 4);

        // add 5 bytes for rip
        relative_offset += 5;

        printf("Relative jmp offst: 0x%08x \n", relative_offset);

        *address = (void*) ((size_t)(*address) + relative_offset);

        printf("Absolute address of function prologue: 0x%08x \n", (size_t)(*address));

        return true;
    }
    return false;
}

void trampoline_hook(void* src, void* dst, void** trampoline_function)
{
    // creating jmp_bytes for src function
    unsigned char* jmp_bytes = NULL;
    create_jump_bytes(&jmp_bytes, dst);

    // for storing overwritten bytes
    unsigned char saved_bytes[HOOK_SIZE];

    // RESOLVE_FUNC
    if (!resolve_func_address(&src))
    {
        printf("Address is not apart of the iat!\n");
    }

    DWORD old_protect = change_protections(src, HOOK_SIZE, PAGE_EXECUTE_READWRITE);

    // save bytes
    memcpy(saved_bytes, src, HOOK_SIZE);
    // overwrite bytes with jmp
    memcpy(src, jmp_bytes, JMP_SIZE);

    // everytime you modify code in memory you must call this
    FlushInstructionCache(GetCurrentProcess(), src, JMP_SIZE);

    printf("Hook installed at: %zx\n", (size_t)src);

    change_protections(src, HOOK_SIZE, old_protect);

    *trampoline_function = VirtualAlloc(NULL, HOOK_SIZE + JMP_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // write saved bytes to new alloced memory for trampoline_func
    memcpy(*trampoline_function, saved_bytes, HOOK_SIZE);
    FlushInstructionCache(GetCurrentProcess(), *trampoline_function, HOOK_SIZE);

    unsigned char* jmp_to_src_bytes = NULL;
    create_jump_bytes(&jmp_to_src_bytes, (void*)(((size_t)src) + HOOK_SIZE));

    size_t trampoline_address = ((size_t)(*trampoline_function)) + HOOK_SIZE;

    // write jmp back to src function, to resume execution
    memcpy((void*)trampoline_address, jmp_to_src_bytes, JMP_SIZE);
    FlushInstructionCache(GetCurrentProcess(), (void*)trampoline_address, JMP_SIZE);

    printf("Trampoline Setup at: %zx\n", trampoline_address - HOOK_SIZE);

    // cleanup
    free(jmp_bytes);
    free(jmp_to_src_bytes);
}
