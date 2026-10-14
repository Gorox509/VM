#ifndef VM_READING_H
#define VM_READING_H

#include "VM.hpp"
#include "../VM_commands.hpp"

int vm_read_file_to_buffer(     FILE *fp, char *buf, size_t block_size);
int vm_translate_buffer_to_code(struct VM_processor *proc,  char *buf,  size_t size);
int vm_read_binary_code(        FILE *fp, struct VM_processor *proc, size_t size);

#endif
