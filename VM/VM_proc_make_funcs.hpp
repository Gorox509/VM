#ifndef VM_PROC_MAKE_H
#define VM_PROC_MAKE_H

#include "VM.hpp"

struct VM_processor *vm_make_processor(         size_t size);
struct VM_processor *vm_make_proc_core();
int                  vm_make_proc_stack(        struct VM_processor *proc);
int                  vm_make_proc_call_stack(   struct VM_processor *proc);
int                  vm_make_proc_code(         struct VM_processor *proc, size_t len);
int                  vm_make_proc_regs(         struct VM_processor *proc);
int                 vm_make_proc_RAM(           struct VM_processor *proc);
ON_VM_DEBUG(int vm_make_proc_error_struct(      struct VM_processor *proc);)

int vm_processor_destructor(                    struct VM_processor *proc);

#endif
