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

const unsigned char rip_relative_opcodes[] = { 0xE9, 0xE8, 0x0D, 0x0F };
const unsigned char jmp_relative_opcodes[] = { 0x80, 0x81, 0x88, 0x89, 0x84, 0x85, 0x82, 0x83, 0x86, 0x87, 0x8C, 0x8D,
                                               0x8E, 0x8F, 0x8A, 0x8B };

const unsigned char opcodes[] = { 0x48, 0x8D };

size_t calculate_absolute_addr(unsigned char** bytes, size_t address, size_t offset, size_t index)
{
    uint32_t rva;
    memcpy(&rva, &(*bytes)[index], 4);

    return rva + offset + address;
}

void handle_relative_jmp(Hook_settings* hk_settings, unsigned char** bytes, void** address, size_t index)
{
    // 77 0C
    // ja rip+0x0C
    // mov rax, absolute_address
    // jmp rax
    size_t absolute_addr = calculate_absolute_addr(bytes, (size_t)(*address), 6, index + 2);

    switch (*bytes[index + 1])
    {
        case 0x85:
            // increment overwrites
            hk_settings->overwrite_count += 1;

            // allocate space for overwrites
            hk_settings->overwrite_bytes = (Overwrite_bytes*)realloc(
                hk_settings->overwrite_bytes, sizeof(Overwrite_bytes) * hk_settings->overwrite_count);

            unsigned char *jmp_bytes = NULL;
            create_jump_bytes(&jmp_bytes, (void*)absolute_addr);
            hk_settings->overwrite_bytes[hk_settings->overwrite_count - 1].bytes = (unsigned char*)malloc(14); // 12 + 2
            hk_settings->overwrite_bytes[hk_settings->overwrite_count - 1].count = 14;
            hk_settings->overwrite_bytes[hk_settings->overwrite_count - 1].index = index;
            // je rip+0xC
            unsigned char ja_bytes[] = { 0x74, 0x0C };

            // copy over our bytes for writing later.
            memcpy(hk_settings->overwrite_bytes[hk_settings->overwrite_count - 1].bytes, ja_bytes, sizeof(ja_bytes));
            memcpy(&hk_settings->overwrite_bytes[hk_settings->overwrite_count - 1].bytes[2], jmp_bytes, 12);

            free(jmp_bytes);
            break;

        default:
            break;
    }
}

void handle_relative_call(Hook_settings* hk_settings, unsigned char** bytes, void** address, size_t index)
{
    size_t absolute_addr = calculate_absolute_addr(bytes, (size_t)(*address), 5, index + 1);

}

void handle_overwrite_bytes();

// going to be working on this to handle any situation, though will probaly take a long time.
Hook_settings calculate_hook_settings(void** address)
{
    Hook_settings hk_settings;

    unsigned char bytes[30];
    DWORD old_protect = change_protections(*address, sizeof(bytes), PAGE_EXECUTE_READ);

    // copy bytes from function prologue
    memcpy(bytes, *address, sizeof(bytes));

    size_t copied_size = 0;

    for (size_t i = 0; i < sizeof(bytes); ++i)
    {
        for (size_t j = 0; j < sizeof(rip_relative_opcodes); ++j)
        {
            if (bytes[i] == rip_relative_opcodes[j])
            {
                switch (bytes[i])
                {
                    case 0x0F:
                        handle_relative_jmp(&hk_settings, &bytes, address, i);
                        break;

                    case 0xE8:

                        break;
                    default:
                        break;
                }
            }
        }

        for (size_t j = 0; j < sizeof(opcodes); ++j)
        {
            if (bytes[i] == opcodes[j])
            {
                switch (bytes[i]) {
                case 0x48:
                    copied_size += i;
                    if (copied_size >= JMP_SIZE)
                    {
                        goto end;
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

end:

    hk_settings.copied_size = copied_size - 1;
    change_protections(*address, sizeof(bytes), old_protect);

    return hk_settings;
}

void install_trampoline_hook(void* src, void* dst, void** trampoline_function)
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
    Hook_settings hk_settings = calculate_hook_settings(&src);

    printf("Calculated hook_size: %zu\n", hk_settings.copied_size);

    // for storing overwritten bytes
    unsigned char *saved_bytes = (unsigned char*)malloc(hk_settings.copied_size);

    DWORD old_protect = change_protections(src, hk_settings.copied_size, PAGE_EXECUTE_READWRITE);

    // save bytes
    memcpy(saved_bytes, src, hk_settings.copied_size);
    // overwrite bytes with jmp
    memcpy(src, jmp_bytes, JMP_SIZE);

    // everytime you modify code in memory you must call this
    FlushInstructionCache(GetCurrentProcess(), src, JMP_SIZE);

    printf("Hook installed at: %zx\n", (size_t)src);

    change_protections(src, hk_settings.copied_size, old_protect);

    *trampoline_function = VirtualAlloc(NULL, hk_settings.copied_size + JMP_SIZE, MEM_COMMIT | MEM_RESERVE,
                                        PAGE_EXECUTE_READWRITE);

    // write saved bytes to new alloced memory for trampoline_func
    memcpy(*trampoline_function, saved_bytes, hk_settings.copied_size);
    FlushInstructionCache(GetCurrentProcess(), *trampoline_function, hk_settings.copied_size);

    unsigned char* jmp_to_src_bytes = NULL;
    create_jump_bytes(&jmp_to_src_bytes, (void*)(((size_t)src) + hk_settings.copied_size));

    size_t trampoline_address = ((size_t)(*trampoline_function)) + hk_settings.copied_size;

    // write jmp back to src function, to resume execution
    memcpy((void*)trampoline_address, jmp_to_src_bytes, JMP_SIZE);
    FlushInstructionCache(GetCurrentProcess(), (void*)trampoline_address, JMP_SIZE);

    printf("Trampoline Setup at: %zx\n", trampoline_address - hk_settings.copied_size);

    // cleanup
    free(jmp_bytes);
    free(jmp_to_src_bytes);
    free(saved_bytes);
}
