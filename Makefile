CC=g++

CFLAGS=-ggdb3 -std=c++17 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations \
-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts\
-Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
-Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self\
-Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 \
-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default \
-Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix\
-Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs\
-Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow \
-flto-odr-type-merging -fno-omit-frame-pointer -pie -fPIE -Werror=vla \
-fsanitize=address,leak,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

DEFINES=-D HASH_PROT -D CANARY_PROT -D STACK_DEBUG -D VM_DEBUG


all:

VM: VM.o stack.o VM_command_funcs.o VM_proc_make_funcs.o VM_reading.o
	@$(CC) $(CFLAGS) $(DEFINES) VM.o stack.o VM_command_funcs.o VM_proc_make_funcs.o VM_reading.o -o vm.out
	@rm -f VM.o stack.o VM_command_funcs.o VM_proc_make_funcs.o VM_reading.o

Compiler: compiler.o compiler_make.o
	@$(CC) $(CFLAGS) $(DEFINES) compiler_make.o compiler.o -o compiler.out
	@rm -f compiler.o compiler_make.o

VM.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) VM/VM.cpp -o VM.o

stack.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) stack/stack.cpp -o stack.o

VM_command_funcs.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) VM/VM_command_funcs.cpp -o VM_command_funcs.o

VM_proc_make_funcs.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) VM/VM_proc_make_funcs.cpp -o VM_proc_make_funcs.o

VM_reading.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) VM/VM_reading.cpp -o VM_reading.o




compiler.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) Compiler/compiler.cpp -o compiler.o

compiler_make.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) Compiler/compiler_make.cpp -o compiler_make.o
