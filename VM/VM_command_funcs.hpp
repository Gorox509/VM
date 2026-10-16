#ifndef VM_CMD_FUNCS_H
#define VM_CMD_FUNCS_H

int vm_do_PUSH(     struct VM_processor *proc);
int vm_do_OUT(      struct VM_processor *proc);
int vm_do_POP(     struct VM_processor *proc);
int vm_do_ADD(      struct VM_processor *proc);
int vm_do_SUB(      struct VM_processor *proc);
int vm_do_MUL(      struct VM_processor *proc);
int vm_do_DIV(      struct VM_processor *proc);

int vm_do_JMP(      struct VM_processor *proc);
int vm_do_JA(       struct VM_processor *proc);
int vm_do_JAE(      struct VM_processor *proc);
int vm_do_JB(       struct VM_processor *proc);
int vm_do_JBE(      struct VM_processor *proc);
int vm_do_JE(       struct VM_processor *proc);
int vm_do_JNE(      struct VM_processor *proc);

int vm_do_IN(       struct VM_processor *proc);
int vm_do_CALL(     struct VM_processor *proc);
int vm_do_RET(      struct VM_processor *proc);

int vm_do_HLT(      struct VM_processor *proc);

#endif
