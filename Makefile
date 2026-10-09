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

DEFINES=-D HASH_PROT -D CANARY_PROT -D STACK_DEBUG


all:

VM: stack.o
	@$(CC) -c $(CFLAGS) $(DEFINES) VM.cpp -o VM.o
	@$(CC) $(CFLAGS) $(DEFINES) VM.o stack.o -o vm
	@rm -f VM.o stack.o

Compiler: compiler.o
	@$(CC) $(CFALGS) $(DEFINES) compiler.o -o compiler
	@rm -f compiler.o

stack.o:
	@$(CC) -c $(CFLAGS) $(DEFINES) stack/stack.cpp -o stack.o

compiler.o:
	@$(CC) -c $(CFALGS) $(DEFINES) compiler.cpp -o compiler.o
