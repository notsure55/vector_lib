#include <Hook/TrampolineHook.h>
#include <Hook/InlineHook.h>
#include <stdbool.h>
#include <stdint.h>

#include <windows.h>

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

        printf("Absolute address of function prologue: 0x%zx \n", (size_t)(*address));

        return true;
    }
    return false;
}

const unsigned char rip_relative_opcodes[3] = { 0xE9, 0xE8, 0x0D };
const unsigned char opcodes[2] = { 0x48, 0x8D };

// going to be working on this to handle any situation, though will probaly take a long time.
size_t calculate_prologue_size(void** address)
{
    unsigned char bytes[30];
    DWORD old_protect = change_protections(*address, sizeof(bytes), PAGE_EXECUTE_READ);

    // copy bytes from function prologue
    memcpy(bytes, *address, sizeof(bytes));

    size_t current_size = 0;

    for (size_t i = 0; i < sizeof(bytes); ++i)
    {
        for (size_t j = 0; j < sizeof(rip_relative_opcodes); ++j)
        {
            if (bytes[i] == rip_relative_opcodes[j])
            {
                printf("Found a rip relative instruction at 0x%zx\n", ((size_t)(*address)) + i);
            }
        }

        for (size_t j = 0; j < sizeof(opcodes); ++j)
        {
            if (bytes[i] == opcodes[j])
            {
                switch (bytes[i]) {
                case 0x48:
                    current_size += i;
                    if (current_size >= JMP_SIZE)
                    {
                        change_protections(*address, sizeof(bytes), old_protect);
                        return (current_size - 1);
                    }
                    break;

                case 0xD0:
                    printf("Found a LEA rip relative instruction at 0x%zx\n", ((size_t)(*address)) + i);
                    break;

                default:
                    break;

                }
            }
        }
    }

    // this is impossible
    printf("30 bytes was not enough!\n");
    return 0;
}

void trampoline_hook(void* src, void* dst, void** trampoline_function)
{
    // creating jmp_bytes for src function
    unsigned char* jmp_bytes = NULL;
    create_jump_bytes(&jmp_bytes, dst);

    // RESOLVE_FUNC
    if (!resolve_func_address(&src))
    {
        printf("Address is not apart of the iat!\n");
    }

    // calculate size of bytes we wont to overwrite
    const size_t hook_size = calculate_prologue_size(&src);

    printf("Calculated hook_size: %zu\n", hook_size);

    // for storing overwritten bytes
    unsigned char *saved_bytes = (unsigned char*)malloc(hook_size);

    DWORD old_protect = change_protections(src, hook_size, PAGE_EXECUTE_READWRITE);

    // save bytes
    memcpy(saved_bytes, src, hook_size);
    // overwrite bytes with jmp
    memcpy(src, jmp_bytes, JMP_SIZE);

    // everytime you modify code in memory you must call this
    FlushInstructionCache(GetCurrentProcess(), src, JMP_SIZE);

    printf("Hook installed at: %zx\n", (size_t)src);

    change_protections(src, hook_size, old_protect);

    *trampoline_function = VirtualAlloc(NULL, hook_size + JMP_SIZE, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    // write saved bytes to new alloced memory for trampoline_func
    memcpy(*trampoline_function, saved_bytes, hook_size);
    FlushInstructionCache(GetCurrentProcess(), *trampoline_function, hook_size);

    unsigned char* jmp_to_src_bytes = NULL;
    create_jump_bytes(&jmp_to_src_bytes, (void*)(((size_t)src) + hook_size));

    size_t trampoline_address = ((size_t)(*trampoline_function)) + hook_size;

    // write jmp back to src function, to resume execution
    memcpy((void*)trampoline_address, jmp_to_src_bytes, JMP_SIZE);
    FlushInstructionCache(GetCurrentProcess(), (void*)trampoline_address, JMP_SIZE);

    printf("Trampoline Setup at: %zx\n", trampoline_address - hook_size);

    // cleanup
    free(jmp_bytes);
    free(jmp_to_src_bytes);
}
