#ifndef TRAMPOLINE_HOOK_H
#define TRAMPOLINE_HOOK_H

void install_trampoline_hook(void* src, void* dst, void** trampoline_function);

#endif //TRAMPOLINE_HOOK_H
